# Covfie install

If you are looking for the code that renders the ATLAS vector field to an image, you should check out https://github.com/acts-project/covfie/blob/main/examples/cpu/render_image.cpp and the CUDA equivalent at https://github.com/acts-project/covfie/blob/main/examples/cuda/render_slice.cu.

To use these examples, here's a rough plan de campagne:

1. Clone the source code (obviously): git clone https://github.com/acts-project/covfie
2. Configure the build with the examples enabled: `cmake -S covfie -B build -DCOVFIE_BUILD_EXAMPLES=On -DCOVFIE_BUILD_TESTS=Off -DCOVFIE_BUILD_BENCHMARKS=On -DCOVFIE_PLATFORM_CPU=On -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20 -DCMAKE_CUDA_STANDARD=17 -DCOVFIE_PLATFORM_CUDA=On`
3. Build the code: `cmake --build build -- -j $(nproc)`
4. Download the ATLAS magnetic field in the ATLAS file format: `wget https://gitlab.cern.ch/acts/acts-data/-/raw/master/MagneticField/ATLAS/ATLASBField_xyz.txt`
5. Convert this file format to a file that we can actually use: `build/examples/core/convert_bfield --input ATLASBField_xyz.txt --output atlas.cvf`
6. Render an image from this file on the CPU by slicing it at z=0 (to get an x-y plane): `build/examples/cpu/render_slice_cpu --input atlas.cvf --output my_image.bmp --z 0 `
7. Do the same on the GPU (this one always slices in z): `build/examples/cuda/render_slice_cuda --input atlas.cvf --output my_image2.bmp`
8. And the same with CUDA texture memory: `build/examples/cuda/render_slice_texture_cuda --input atlas.cvf --output my_image3.bmp`

The benchmarks/cpu/patterns/lorentz_euler.hpp is also interesting, but does slightly different things. It spawns a number of agents with random momentum, and propagates them through the magnetic field

Which is designed to be a benchmark for a more realistic physics application

You can run that with COVFIE_BENCHMARK_ATLAS_FIELD=atlas.cvf build/benchmarks/cuda/benchmark_cuda
