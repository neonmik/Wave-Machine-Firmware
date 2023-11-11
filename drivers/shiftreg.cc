#include "shiftreg.h"

#include "sn74595.pio.h"

#define NUMBER_OF_DISP_ROWS 8

int shiftRegDMAChannel; 
namespace ShiftReg {



    // void shiftRegDMAHandler() {
    //     dma_hw->ints0 = 1u << shiftRegDMAChannel;
    //     dma_channel_set_read_addr(shiftRegDMAChannel, &buffer, true);
    // }
    // 
    // void DMAinit (void) {
    //     shiftRegDMAChannel = dma_claim_unused_channel(true); 
    //     dma_channel_config ShiftRegDMAConfig = dma_channel_get_default_config(shiftRegDMAChannel);
    //     channel_config_set_transfer_data_size(&ShiftRegDMAConfig, DMA_SIZE_8); 
    //     channel_config_set_read_increment(&ShiftRegDMAConfig, true);
    //     channel_config_set_write_increment(&ShiftRegDMAConfig, false); 
    //     channel_config_set_dreq(&ShiftRegDMAConfig, DREQ_PIO0_TX1);
    //     channel_config_set_ring(&ShiftRegDMAConfig, false, 2);
    // 
    //     dma_channel_configure(
    //         shiftRegDMAChannel,
    //         &ShiftRegDMAConfig,
    //         &pio0_hw->txf[1],
    //         buffer,
    //         MAX_RESOLUTION,
    //         true
    //     );
    // 
    //     irq_set_exclusive_handler(DMA_IRQ_1, shiftRegDMAHandler);
    //     dma_channel_set_irq1_enabled(shiftRegDMAChannel, true);
    //     irq_set_enabled(DMA_IRQ_1, true);
    // }

    void init() {
        gpio_set_drive_strength(SRPins::SR_CLK, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(SRPins::SR_DATA, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(SRPins::SR_LATCH, GPIO_DRIVE_STRENGTH_2MA);

        gpio_set_slew_rate(SRPins::SR_CLK, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(SRPins::SR_DATA, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(SRPins::SR_LATCH, GPIO_SLEW_RATE_SLOW);

        sn74595::shiftreg_init();

        update();
    }
    void update(void) {
        if (!needsSending) return;
        
        for (int i = 0; i < 8; i++) {
            sn74595::setOutput(0, i, (buffer[0] & (1 << i)));
        }
        sn74595::sendOutput();

        // time++;
        // if (time > resolution) time = 0;

        needsSending = false;
    }

    void set_bit(Pins pin, bool value) {
        uint8_t temp = static_cast<uint8_t>(pin);
        if (value) {
            buffer[0] |= temp;
        } else {
            buffer[0] &= ~temp;
        }
        needsSending = true;
    }
    
    // void set_bit_level(Pins pin, bool value, uint8_t level) {
    //     uint8_t temp = static_cast<uint8_t>(pin);
    //     for (int i = 0; i < MAX_RESOLUTION; i++) {
    //         buffer[i] &= ~temp; // clear pin
    //         if ((i <= level) && value) {
    //             buffer[i] |= temp;
    // 
    //         } else {
    //             buffer[i] &= ~temp;
    //         }
    //     }
    // }

    void off (void) {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        buffer[0] &= ~temp;
        needsSending = true;
    }

    void clear (void) {
        off();
        update();
    }
}