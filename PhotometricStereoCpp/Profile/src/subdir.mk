################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Display.cpp \
../src/GaussSeidelSolver.cpp \
../src/ImageGrabber.cpp \
../src/InputAdjuster.cpp \
../src/LightingPanel.cpp \
../src/ModelRenderer.cpp \
../src/PhotometricStereo.cpp \
../src/PhotometricStereoCpp.cpp 

OBJS += \
./src/Display.o \
./src/GaussSeidelSolver.o \
./src/ImageGrabber.o \
./src/InputAdjuster.o \
./src/LightingPanel.o \
./src/ModelRenderer.o \
./src/PhotometricStereo.o \
./src/PhotometricStereoCpp.o 

CPP_DEPS += \
./src/Display.d \
./src/GaussSeidelSolver.d \
./src/ImageGrabber.d \
./src/InputAdjuster.d \
./src/LightingPanel.d \
./src/ModelRenderer.d \
./src/PhotometricStereo.d \
./src/PhotometricStereoCpp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -g -pg -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


