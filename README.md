# ebpf-sched-interface

## Dependency

Operating System: Ubuntu 18.04.5 LTS with modified Linux kernel 5.8.0

GCC/G++: 7.5


## Compile Kernel

```bash
# enable deb-src
sudo cp /etc/apt/sources.list /etc/apt/sources.list~
sudo sed -Ei 's/^# deb-src /deb-src /' /etc/apt/sources.list
sudo apt-get update

# install build dependency
sudo apt-get build-dep linux linux-image-$(uname -r) -y
sudo apt-get install libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf fakeroot -y
```

Then, clone the Linux repository and checkout to 5.8:

```bash
git clone https://github.com/torvalds/linux.git
cd linux
git checkout tags/v5.8
```

Apply the kernel patch you need and compile the modified kernel:

```bash
git apply patch
make localmodconfig
make deb-pkg
```

After the kernel is successfully compiled, install all the  `.deb`  files generated in the parent folder of  `linux`:

```bash
cd ..
sudo dpkg -i *.deb
```

Finally, reboot the machine and make sure that you boot into the right kernel. You can examine your current kernel by running `uname -r` and boot into another kernel using `grub-reboot` with a reboot.


## Load BPF Program

First, install the dependencies for building and loading BPF programs:

```bash
sudo apt update
sudo apt install gcc-multilib clang llvm libelf-dev libdwarf-dev -y

wget http://archive.ubuntu.com/ubuntu/pool/universe/libb/libbpf/libbpf0_0.1.0-1_amd64.deb
wget http://archive.ubuntu.com/ubuntu/pool/universe/libb/libbpf/libbpf-dev_0.1.0-1_amd64.deb
sudo dpkg -i libbpf0_0.1.0-1_amd64.deb
sudo dpkg -i libbpf-dev_0.1.0-1_amd64.deb
```

Then, run the script provided in this repository to compile and load the BPF program before running the benchmarks:

```bash
cd bpf
sudo ./load_bpf.sh
```

## Run Benchmark

First, compile the benchmark programs:

```bash
g++ -o -lrt bench.c -o bench.o
./bench.o <num_threads> # runs with 8 independent threads by default
```
