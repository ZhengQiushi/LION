
#include <algorithm>
#include "brain/testing_forecast_util.h"
#include "brain/util/model_util.h"
#include "brain/workload/kernel_model.h"
#include "brain/workload/linear_model.h"
#include "brain/workload/lstm.h"
#include "brain/workload/workload_defaults.h"
#include "common/harness.h"
#include "glog/logging.h"
#include <fstream>


#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <map>
#include <glog/logging.h>

#include "core/Defs.h"
#include "common/internal_types.h"
#include "brain/util/model_util.h"
#include "brain/util/eigen_util.h"
#include "brain/util/tf_util.h"
#include "brain/workload/base_tf.h"
#include "brain/workload/workload_defaults.h"

#include "brain/workload_forcast.h"


// #include "util/file_util.h"

using  namespace lion; // {
using  namespace test;// {
class ModelTests : public lionTest {};
    lion::matrix_eig GetWorkload(size_t& num_samples, size_t& num_feats) {
        lion::matrix_eig data;
        data = lion::matrix_eig::Zero(num_samples, num_feats);

        std::ifstream ifs("/home/star/data/getWorkLoad.xls", std::ios::in);
        int row_cnt = 0;
        std::string _line = "";

        while (getline(ifs, _line)){
            //解析每行的数据
            std::stringstream ss(_line);
            std::string _sub;
            std::vector<std::string> subArray;

            //按照逗号分隔
            while (getline(ss, _sub, '\t'))
                subArray.push_back(_sub);
            
            // 
            for (size_t i=0; i<subArray.size() && i < num_feats; ++i) {
                data(row_cnt, i) = atof(subArray[i].data());
            }
            // 
            row_cnt ++ ;
            if(row_cnt >= num_samples){
                break;
            }

        }

        // std::cout << data << std::endl;

        char prompt[50] = {0};
        sprintf(prompt, "Generating a workload of dims: %ld x %ld", num_samples, num_feats);
        LOG(INFO) << prompt;
        
        return data;
    }


    /**
     * @brief 
     * 
     * @param model 
     * @param w 
     * @param val_interval 
     * @param num_samples 
     * @param num_feats 
     * @param val_split 
     * @param normalize 
     * @param val_loss_thresh 
     * @param early_stop_patience 
     * @param early_stop_delta 
     */
    
    void WorkloadTest(
            lion::brain::BaseForecastModel &model, size_t val_interval,
            size_t num_samples, size_t num_feats, float val_split, bool normalize,
            float val_loss_thresh, size_t early_stop_patience, float early_stop_delta) {

        // size_t val_interval = 20; 
        // size_t num_samples = 1000;
        // size_t num_feats = 3;
        // float val_split = 0.5;
        // bool normalize = false;
        // float val_loss_thresh = 0.05;
        
        LOG(INFO) << "Using Model: " << (model.ToString().data());

        lion::matrix_eig data = GetWorkload(num_samples, num_feats);
        std::cout << data << std::endl;
        // std::cout << data << std::endl;
        lion::brain::Normalizer n(normalize);
        val_interval = std::min<size_t>(val_interval, model.GetEpochs());

        // Determine the split point
        size_t split_point =
            data.rows() - static_cast<size_t>(data.rows() * val_split);

        // Split into train/test data
        lion::matrix_eig train_data = data.topRows(split_point);
        n.Fit(train_data);
        train_data = n.Transform(train_data);

        // test_data for validate
        lion::matrix_eig test_data = 
            n.Transform(data.bottomRows(
                static_cast<size_t>(data.rows() - split_point)));

        vector_eig train_loss_avg = vector_eig::Zero(val_interval);
        float prev_train_loss = std::numeric_limits<float>::max();
        float val_loss = val_loss_thresh * 2;
        std::vector<float> val_losses;
        for (int epoch = 1; epoch <= model.GetEpochs() &&
                            !brain::ModelUtil::EarlyStop(
                                val_losses, early_stop_patience, early_stop_delta);
            epoch++) {
            auto train_loss = model.TrainEpoch(train_data);
            size_t idx = (epoch - 1) % val_interval;
            train_loss_avg(idx) = train_loss;
            if (epoch % val_interval == 0) {
                val_loss = model.ValidateEpoch(test_data);
                train_loss = train_loss_avg.mean();
                // Below check is not advisable - one off failure chance
                // EXPECT_LE(val_loss, prev_valid_loss);
                // An average on the other hand should surely pass
                // DCHECK(train_loss < prev_train_loss);
                LOG(WARNING) <<  "Train Loss: " << train_loss << " Prev Train Loss: " << val_loss;
                LOG(INFO) << "Train Loss: " << train_loss <<", Valid Loss: " << val_loss;
                prev_train_loss = train_loss;
            }
        }
        LOG(INFO) << "Training done.";
        DCHECK(val_loss < val_loss_thresh);

        matrix_eig y, y_hat;
        val_loss = model.ValidateEpoch(test_data, y, y_hat);
        matrix_eig C(y.rows(), y_hat.cols() + y_hat.cols());
        C << y, y_hat;

        float mean_, std_, min_;
        n.GetParameters(mean_, std_, min_);
        // matrix_eig C_ = C.unaryExpr([&](double x){
        //     return std::exp(x* std_ + mean_) - min_;
        // }).cast<float>();
        
        LOG(INFO) << "Predict done.";
        /** output y and y_hat**/
        std::ofstream ofs("/home/star/data/y_hat.xls", std::ios::trunc);
        for(int i = 0; i < C.rows(); i ++ ){
            for(int j = 0; j < C.cols(); j ++ ){
                ofs << C(i, j) << "\t";
            }
            ofs << "\n";
        }
        ofs.close();

    }

// Enable after resolving
TEST_F(ModelTests, DISABLED_TimeSeriesLSTMTest) { // 
    int PREIOD = 160; // 40 / 0.25
    int EPOCHS = 100;
    int CLIP_NORM = 0.25;
    int BPTT = PREIOD * 2;
    int HORIZON = PREIOD * 3;
    int INTERVAL = 20;
    int BATCH_SIZE = 4;

    auto model = std::unique_ptr<lion::brain::TimeSeriesLSTM>(new lion::brain::TimeSeriesLSTM(
      lion::brain::LSTMWorkloadDefaults::NFEATS,
      lion::brain::LSTMWorkloadDefaults::NENCODED, lion::brain::LSTMWorkloadDefaults::NHID,
      lion::brain::LSTMWorkloadDefaults::NLAYERS, lion::brain::LSTMWorkloadDefaults::LR,
      lion::brain::LSTMWorkloadDefaults::DROPOUT_RATE,
      lion::brain::LSTMWorkloadDefaults::CLIP_NORM,
      lion::brain::LSTMWorkloadDefaults::BATCH_SIZE,
      BPTT, HORIZON,
      lion::brain::CommonWorkloadDefaults::INTERVAL,
      EPOCHS));

    DCHECK(model->IsTFModel() == true);
    size_t LOG_INTERVAL = 20;
    size_t NUM_SAMPLES = 1600;
    size_t NUM_FEATS = 3;
    float VAL_SPLIT = 0.5;
    bool NORMALIZE = true;
    float VAL_THESH = 0.05;

    WorkloadTest(*model, 
                 LOG_INTERVAL, NUM_SAMPLES, NUM_FEATS,
                 VAL_SPLIT, NORMALIZE, VAL_THESH,
                 lion::brain::CommonWorkloadDefaults::ESTOP_PATIENCE,
                 lion::brain::CommonWorkloadDefaults::ESTOP_DELTA);
}


TEST_F(ModelTests, TimeSeriesLSTMTestClass) {
    int preiod = 90 / 0.5;
    int bptt = 1 * preiod; 
    int horizon = 3 * preiod;

    size_t num_samples = 10 * preiod;
    size_t num_feats = 3;

    float val_split = 0.5;
    lion::brain::my_predictor p_(preiod, num_feats, bptt, horizon, val_split);

    lion::matrix_eig data = GetWorkload(num_samples, num_feats);
    // std::cout << data << std::endl;
    
    p_.train(data);
    p_.predict(data);
}


int main(int argc, char **argv)
{

  // 分析gtest程序的命令行参数
  testing::InitGoogleTest(&argc, argv);

  // 调用RUN_ALL_TESTS()运行所有测试用例
  // main函数返回RUN_ALL_TESTS()的运行结果

  int rc = RUN_ALL_TESTS();

  return rc;
}
// }  // namespace test
// }  // namespace lion
