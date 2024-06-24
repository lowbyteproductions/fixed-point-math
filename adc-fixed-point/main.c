#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define FP_INT_TYPE      int32_t
#define FP_INT_XL_TYPE   int64_t
#define FP_INT_BITS      (14)
// For header-only library, define only once
#define FP_LIB_IMPL
#include "fp.h"

static float map_range(float a, float b, float c, float d, float v) {
    return (v-a)/(b-a) * (d-c) + c;
}

static fp_int_t fp_map_range(fp_int_t a, fp_int_t b, fp_int_t c, fp_int_t d, fp_int_t v) {
    return fp_mul(fp_div((v-a),(b-a)), (d-c)) + c;
}

// 1 second of data
#define NUM_ADC_SAMPLES (2048)
int32_t ADCSamples[NUM_ADC_SAMPLES] = {0};
static void ComputeADCSamples(void) {
    const float pi = 3.1415926f;
    const float two_pi = 2.0f * pi;
    const float low_freq = 10.0f;
    const float high_freq = 303.0f;
    const float low_gain = 1.0f;
    const float high_gain = 0.2f;
    const float max_gain = fabsf(low_gain) + fabsf(high_gain);

    for (int i = 0; i < NUM_ADC_SAMPLES; i++) {
        float t = (float)i / (float)(NUM_ADC_SAMPLES - 1);
        float value = low_gain * sinf(two_pi * t * low_freq) + high_gain * sinf(two_pi * t * high_freq);
        ADCSamples[i] = (int32_t)map_range(-max_gain, max_gain, 0, (1 << 12)-1, value);
    }
}

static fp_int_t ADCToFP(int32_t sample) {
    const fp_int_t ref_voltage = float_to_fp(3.3f);
    const fp_int_t adc_max = FP_FROM_INT(4095);
    fp_int_t adc_fp = FP_FROM_INT(sample);
    return fp_mul(ref_voltage, fp_div(adc_fp, adc_max));
}

int main() {
    ComputeADCSamples();

    // alpha = 0.030211439039808382 beta = 0.9697885609601916
    // alpha_fp = 7919, beta_fp = 254224
    // 5569, beta_fp = 256574
    const fp_int_t alpha = 5569;
    const fp_int_t beta = 256574;
    const fp_int_t ref_voltage = float_to_fp(3.3f);

    fp_int_t filtered = ADCToFP(ADCSamples[0]);
    fp_int_t pwm = 0;

    for (int i = 1; i < NUM_ADC_SAMPLES; i++) {
        fp_int_t sample = ADCToFP(ADCSamples[i]);
        filtered = fp_mul(alpha, sample) + fp_mul(beta, filtered);

        pwm = fp_map_range(0, ref_voltage, 0, 255, filtered);
        printf("%d\n", pwm);
    }

    return 0;
}
