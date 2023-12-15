#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

const unsigned int LO = 22;
const unsigned int NUM_ITER = 5;

void addArrayGPU(NSArray<NSNumber *> *arr1, NSArray<NSNumber *> *arr2, unsigned int N) {

    NSArray<id<MTLDevice>> *devices = MTLCopyAllDevices();
    id<MTLDevice> device = [devices firstObject];

    // fifo queue for sending commands to the gpu
    id<MTLCommandQueue> commandQueue = [device newCommandQueue];

    // library for getting our metal functions
    id<MTLLibrary> gpuFunctionLibrary = [device newDefaultLibrary];

    // grab our gpu function
    id<MTLFunction> additionGPUFunction = [gpuFunctionLibrary newFunctionWithName:@"add_arrays"];

    id<MTLComputePipelineState> additionComputePipelineState;
    NSError *error = nil;
    additionComputePipelineState = [device newComputePipelineStateWithFunction:additionGPUFunction error:&error];
    if (error) {
        NSLog(@"%@", error);
    }

    // Create the buffers to be sent to the gpu from our arrays
    float *arr1Elements = malloc(sizeof(float) * N);
    float *arr2Elements = malloc(sizeof(float) * N);
    for (NSInteger i = 0; i < N; i++) {
        arr1Elements[i] = arr1[i].floatValue;
        arr2Elements[i] = arr2[i].floatValue;
    }

    id<MTLBuffer> arr1Buff = [device newBufferWithBytes:arr1Elements
                                                 length:sizeof(float) * N
                                                options:MTLResourceStorageModeShared];

    id<MTLBuffer> arr2Buff = [device newBufferWithBytes:arr2Elements
                                                 length:sizeof(float) * N
                                                options:MTLResourceStorageModeShared];

    id<MTLBuffer> resultBuff = [device newBufferWithLength:sizeof(float) * N
                                                  options:MTLResourceStorageModeShared];

    // create a buffer to be sent to the command queue
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];

    // create an encoder to set values on the compute function
    id<MTLComputeCommandEncoder> commandEncoder = [commandBuffer computeCommandEncoder];
    [commandEncoder setComputePipelineState:additionComputePipelineState];

    // set the parameters of our gpu function
    [commandEncoder setBuffer:arr1Buff offset:0 atIndex:0];
    [commandEncoder setBuffer:arr2Buff offset:0 atIndex:1];
    [commandEncoder setBuffer:resultBuff offset:0 atIndex:2];

    // dispatch threads
    MTLSize threadsPerGrid = MTLSizeMake(N, 1, 1);
    NSUInteger maxThreadsPerThreadgroup = additionComputePipelineState.maxTotalThreadsPerThreadgroup;
    MTLSize threadsPerThreadgroup = MTLSizeMake(maxThreadsPerThreadgroup, 1, 1);
    [commandEncoder dispatchThreads:threadsPerGrid
              threadsPerThreadgroup:threadsPerThreadgroup];

    // end encoding and commit the command buffer
    [commandEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];

    // clean up
    free(arr1Elements);
    free(arr2Elements);
}


void addArrayCPU(NSArray<NSNumber *> *arr1, NSArray<NSNumber *> *arr2, unsigned int N) {

    // initialize result array with zeros
    NSMutableArray<NSNumber *> *result = [NSMutableArray arrayWithCapacity:N];
    for (NSInteger i = 0; i < N; i++) {
        [result addObject:@0.0];
    }

    for (NSInteger i = 0; i < N; i++) {
        float sum = [arr1[i] floatValue] + [arr2[i] floatValue];
        result[i] = @(sum);
    }
}

NSArray<NSNumber *> *getRandomArray(NSInteger count) {
    srand((unsigned int)time(NULL));
    
    NSMutableArray<NSNumber *> *result = [NSMutableArray arrayWithCapacity:count];

    for (NSInteger i = 0; i < count; i++) {
        float randomValue = (float)rand()/(float)(RAND_MAX);
        [result addObject:@(randomValue)];
    }

    return result;
}

void recordPerformance(void) {
    unsigned int N = 1 << LO;
    
    NSMutableArray *NValues = [NSMutableArray arrayWithCapacity:NUM_ITER];
    NSMutableArray *gpuTimes = [NSMutableArray arrayWithCapacity:NUM_ITER];
    NSMutableArray *cpuTimes = [NSMutableArray arrayWithCapacity:NUM_ITER];

    
    for (NSInteger i = 0; i < NUM_ITER; i++) {
        NSArray<NSNumber *> *array1 = getRandomArray(N);
        NSArray<NSNumber *> *array2 = getRandomArray(N);
        
        CFAbsoluteTime startGPU = CFAbsoluteTimeGetCurrent();
        addArrayGPU(array1, array2, N);
        CFAbsoluteTime endGPU = CFAbsoluteTimeGetCurrent();
        [gpuTimes addObject:@(endGPU - startGPU)];
        
        CFAbsoluteTime startCPU = CFAbsoluteTimeGetCurrent();
        addArrayCPU(array1, array2, N);
        CFAbsoluteTime endCPU = CFAbsoluteTimeGetCurrent();
        [cpuTimes addObject:@(endCPU - startCPU)];

        [NValues addObject:@(N)];
        N <<= 1;
    }
    
    NSString *NString = [NValues componentsJoinedByString:@","];
    NSString *gpuTimeString = [gpuTimes componentsJoinedByString:@","];
    NSString *cpuTimeString = [cpuTimes componentsJoinedByString:@","];
    NSLog(@"add_array_gpu/%@/%@", NString, gpuTimeString);
    NSLog(@"add_array_cpu/%@/%@", NString, cpuTimeString);
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        recordPerformance();
    }
    return 0;
}
