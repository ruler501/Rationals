/**********************************************************************
Copyright ©2013 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <functional>
#include <ctime>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <vector>
#include <thread>
#include <cmath>
#include <string>
#include <algorithm>
#include <thread>
#include <cmath>
#include <sstream>

#define SUCCESS 0
#define FAILURE 1
#define EXPECTED_FAILURE 2

const int NUM_ELEMENTS = 32768;

void printOutput(unsigned long long start, std::vector<unsigned long long*> values){
    int cur=0;
    for(unsigned int i = 0; i < NUM_ELEMENTS*values.size(); i++){
        //if(values[cur][i%NUM_ELEMENTS] != 0)
            std::cout << start+i << ',' << values[cur][i%NUM_ELEMENTS] << std::endl;
        if(i%NUM_ELEMENTS == NUM_ELEMENTS-1) cur++;
    }
}

void newList(unsigned long long start, unsigned long long *dataList){
    for(int i=0; i < NUM_ELEMENTS; ++i)
        dataList[i] = start + i;
}

using namespace cl;

std::vector<Kernel> kernels;
Context context;
std::vector<CommandQueue> queue;
std::vector<Buffer> inputBuffer;
std::vector<Buffer> outputBuffer;
NDRange global(NUM_ELEMENTS/2);
NDRange local(256);

int init() {
    cl_int status = 0;
	const char* buildOption ="-x clc++ ";
	std::vector<Platform> platforms;
	status = Platform::get(&platforms);
	if (status != CL_SUCCESS){
		std::cout<<"Error: Getting platforms!"<<std::endl;
		return FAILURE;
	}
	std::vector<cl::Platform>::iterator iter;
	for(iter = platforms.begin(); iter != platforms.end(); ++iter)
		if(!strcmp((*iter).getInfo<CL_PLATFORM_VENDOR>().c_str(), "Advanced Micro Devices, Inc."))
            break;
	cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(*iter) (), 0};
	bool gpuNotFound = false;
	try{
		context = cl::Context(CL_DEVICE_TYPE_GPU, cps, NULL, NULL, &status);
	}
	catch(std::exception e){
		gpuNotFound = true;
	}
	if(gpuNotFound){
		std::cout<<"GPU not found, falling back to CPU!"<<std::endl;
		context = cl::Context(CL_DEVICE_TYPE_CPU, cps, NULL, NULL, &status);
		if (status != CL_SUCCESS){
			std::cout<<"Error: Creating context!"<<std::endl;
			return FAILURE;
		}
	}
	Program program;

    std::vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    try{
		for(unsigned int i=0; i < devices.size(); i++){
            CommandQueue tempQueue = CommandQueue(context, devices[i]);
            std::ifstream sourceFile("Rationals.cl");
            std::string sourceCode(
                std::istreambuf_iterator<char>(sourceFile),
                (std::istreambuf_iterator<char>()));
            Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
            program = Program(context, source);
            program.build(devices, buildOption);
            Kernel tempKernel = Kernel(program, "countRationals");
            Buffer tempInputBuffer = Buffer(context, CL_MEM_READ_WRITE, NUM_ELEMENTS * sizeof(unsigned long long));
            Buffer tempOutputBuffer = Buffer(context, CL_MEM_READ_WRITE, NUM_ELEMENTS * sizeof(unsigned long long));
            queue.push_back(tempQueue);
            kernels.push_back(tempKernel);
            inputBuffer.push_back(tempInputBuffer);
            outputBuffer.push_back(tempOutputBuffer);
		}
    }catch(cl::Error e){
        std::cout << e.what() << std::endl;
        std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(cl::Device::getDefault()) << std::endl;
        std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(cl::Device::getDefault()) << std::endl;
        std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault()) << std::endl;
        return FAILURE;
    }
	return SUCCESS;
}

int execute(unsigned long long* inputList, unsigned long long* outputList, unsigned int i) {
    try{
		queue[i].enqueueWriteBuffer(inputBuffer[i], CL_TRUE, 0, NUM_ELEMENTS * sizeof(unsigned long long), inputList);
		kernels[i].setArg(0, inputBuffer[i]);
		kernels[i].setArg(1, outputBuffer[i]);
		queue[i].enqueueNDRangeKernel(kernels[i], NullRange, global, local);
		queue[i].enqueueReadBuffer(outputBuffer[i], CL_TRUE, 0, NUM_ELEMENTS * sizeof(unsigned long long), outputList);
	}catch(cl::Error e){
		std::cout << "Line "<< __LINE__<<": Error in "<<e.what() <<std::endl;
		return FAILURE;
	}
    return SUCCESS;
}

using namespace std;

int main(int argc, char* argv[]){
    unsigned long long minNum, maxNum;
    if(argc == 1){
        minNum = pow(3, 10);
        maxNum = pow(3, 11);
    }
    else if(argc == 2){
        minNum = pow(3, atoi(argv[1]));
        maxNum = pow(3, atoi(argv[1]) + 1);
    }
    else if(argc == 3){
        minNum = pow(3, atoi(argv[1]));
        maxNum = pow(3, atoi(argv[2]));
    }
    else return -1;
    init();
    vector<unsigned long long*>  q(kernels.size()), result(kernels.size()), old(kernels.size()), newq(kernels.size());
    unsigned long long *temp = nullptr;
    vector<thread> workThread(kernels.size()), genThread(kernels.size());
    thread outThread;
    for(unsigned int i = 0; i < kernels.size(); i++){
        q[i] = new unsigned long long[NUM_ELEMENTS];
        newq[i] = new unsigned long long[NUM_ELEMENTS];
        old[i] = new unsigned long long[NUM_ELEMENTS];
        result[i] = new unsigned long long[NUM_ELEMENTS];
    }
    for(unsigned int i = 0; i < kernels.size(); i++){
        newList(minNum+NUM_ELEMENTS*i, q[i]);
        workThread[i] = thread(execute, q[i], result[i], i);
        genThread[i] = thread(newList, minNum+NUM_ELEMENTS*(i+kernels.size()), newq[i]);
    }
    for(unsigned int i = 0; i < kernels.size(); i++){
        workThread[i].join();
        genThread[i].join();
    }
    for(unsigned long long i = minNum + NUM_ELEMENTS; i < maxNum  + kernels.size()*NUM_ELEMENTS; i += kernels.size()*NUM_ELEMENTS){
        for(unsigned int j = 0; j < kernels.size(); j++){
            temp = result[j];
            result[j] = old[j];
            old[j] = temp;
            temp = q[j];
            q[j] = newq[j];
            newq[j] = temp;
            temp = nullptr;
        }
        for(unsigned int j = 0; j < kernels.size(); j++){
            genThread[j] = thread(newList, i+NUM_ELEMENTS*(kernels.size()+j), newq[j]);
            workThread[j] = thread(execute, q[j], result[j], j);
        }
        outThread = thread(printOutput, i-NUM_ELEMENTS*kernels.size(), old);
        for(unsigned int j = 0; j < kernels.size(); j++){
            workThread[j].join();
            genThread[j].join();
        }
        outThread.join();
    }
    return 0;
}
