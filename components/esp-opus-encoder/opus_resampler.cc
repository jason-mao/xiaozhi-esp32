#include "opus_resampler.h"
#include "silk_resampler.h"
#include "esp_log.h"
#include "esp_ae_rate_cvt.h"

#define TAG "OpusResampler"
static esp_ae_rate_cvt_handle_t esp_rate;

OpusResampler::OpusResampler() {
}

OpusResampler::~OpusResampler() {
    esp_ae_rate_cvt_close(esp_rate);
}

#if 0
void OpusResampler::Configure(int input_sample_rate, int output_sample_rate) {
    int encode = input_sample_rate > output_sample_rate ? 1 : 0;
    auto ret = silk_resampler_init(&resampler_state_, input_sample_rate, output_sample_rate, encode);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to initialize resampler");
        return;
    }
    input_sample_rate_ = input_sample_rate;
    output_sample_rate_ = output_sample_rate;
    ESP_LOGI(TAG, "Resampler configured with input sample rate %d and output sample rate %d", input_sample_rate_, output_sample_rate_);
}

void OpusResampler::Process(const int16_t *input, int input_samples, int16_t *output) {
    auto ret = silk_resampler(&resampler_state_, output, input, input_samples);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to process resampler");
    }
}

#else

void OpusResampler::Configure(int input_sample_rate, int output_sample_rate) {
    esp_ae_rate_cvt_cfg_t cfg = {
        .src_rate = (uint32_t)input_sample_rate,
        .dest_rate = (uint32_t)output_sample_rate,
        .channel = 1,
        .bits_per_sample = 16,
        .complexity = 1,
        .perf_type = ESP_AE_RATE_CVT_PERF_TYPE_SPEED,
    };
    auto ret = esp_ae_rate_cvt_open(&cfg, &esp_rate);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to initialize resampler");
        return;
    }
    input_sample_rate_ = input_sample_rate;
    output_sample_rate_ = output_sample_rate;
    ESP_LOGE(TAG, "Resampler configured with input sample rate %d and output sample rate %d", input_sample_rate_, output_sample_rate_);
}

void OpusResampler::Process(const int16_t *input, int input_samples, int16_t *output) {
    uint32_t out_samples= 2880;
    auto ret  = esp_ae_rate_cvt_process(esp_rate, (void*)input, input_samples, output, &out_samples);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to process resampler");
    }
}

#endif

int OpusResampler::GetOutputSamples(int input_samples) const {
    return input_samples * output_sample_rate_ / input_sample_rate_;
}

