#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

const unsigned int N = 1 << 24;

void addArrayGPU(NSArray<NSNumber *> *arr1, NSArray<NSNumber *> *arr2) {
    CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();

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

    NSLog(@"\nAdding arrays of size %d in GPU way", N);

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

    float *resultBufferPointer = (float *)[resultBuff contents];

    // optionally, verify the first 5 results
    for (NSInteger i = 0; i < 5; i++) {
        NSLog(@"%f + %f = %f", arr1Elements[i], arr2Elements[i], resultBufferPointer[i]);
    }

    // clean up
    free(arr1Elements);
    free(arr2Elements);

    CFAbsoluteTime timeElapsed = CFAbsoluteTimeGetCurrent() - startTime;
    NSLog(@"Time elapsed %.05f seconds", timeElapsed);
    NSLog(@"");
}


void addArrayCPU(NSArray<NSNumber *> *arr1, NSArray<NSNumber *> *arr2) {
    NSLog(@"Adding arrays of size %d in CPU way", N);
    
    CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();

    // initialize result array with zeros
    NSMutableArray<NSNumber *> *result = [NSMutableArray arrayWithCapacity:N];
    for (NSInteger i = 0; i < N; i++) {
        [result addObject:@0.0];
    }

    for (NSInteger i = 0; i < N; i++) {
        float sum = [arr1[i] floatValue] + [arr2[i] floatValue];
        result[i] = @(sum);
    }
    
    // optionally, verify the first 5 results
    for (NSInteger i = 0; i < 5; i++) {
        NSLog(@"%@ + %@ = %@", arr1[i], arr2[i], result[i]);
    }
    
    CFAbsoluteTime timeElapsed = CFAbsoluteTimeGetCurrent() - startTime;

    NSLog(@"Time elapsed %.02f seconds", timeElapsed);
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

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSArray<NSNumber *> *array1 = getRandomArray(N);
        NSArray<NSNumber *> *array2 = getRandomArray(N);
        addArrayCPU(array1, array2);
        addArrayGPU(array1, array2);
    }
    return 0;
}
