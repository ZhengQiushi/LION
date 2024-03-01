[Lion: Minimizing Distributed Transactions through
Adaptive Replica Provision](https://anonymous.4open.science/r/LION-EXTENED-2024/LION-ICDE24-EXTENDED.pdf)


# Dependencies

```sh
sudo apt-get update
sudo apt-get install -y zip make cmake g++ libjemalloc-dev libboost-dev libgoogle-glog-dev
```

# Download

```sh
git clone https://github.com/ZhengQiushi/LION.git
```

# Install from docker
```sh
docker pull 413366511/ubuntu
./install_new.sh
```

# Build

```
./compile.sh
```

# Run
```
bash /home/star/scripts/test.sh 1_1_s_dist_ratio/uniform/lionS/aws_100.py 240
```
