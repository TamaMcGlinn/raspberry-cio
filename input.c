#include <bcm2835.h>
#include <stdio.h>
#include <assert.h>

#define BUTTON_PIN RPI_GPIO_P1_12

#define ENCODER_A_PIN RPI_GPIO_P1_16
#define ENCODER_B_PIN RPI_GPIO_P1_18

// 00 -> 01 -> 11 -> 10
int rotate_diff[16];

int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
    // Use for testing
//    bcm2835_set_debug(1);

    if (!bcm2835_init())
	return 1;

    // Set RPI pin P1-12 to be an input with pullup resistor
    bcm2835_gpio_fsel(BUTTON_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BUTTON_PIN, BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(ENCODER_A_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(ENCODER_A_PIN, BCM2835_GPIO_PUD_DOWN);

    bcm2835_gpio_fsel(ENCODER_B_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(ENCODER_B_PIN, BCM2835_GPIO_PUD_DOWN);

    // identity
    rotate_diff[0b0000] = 0;
    rotate_diff[0b0101] = 0;
    rotate_diff[0b1010] = 0;
    rotate_diff[0b1111] = 0;

    // up
    rotate_diff[0b0001] = 1;
    rotate_diff[0b0111] = 1;
    rotate_diff[0b1000] = 1;
    rotate_diff[0b1110] = 1;

    // down
    rotate_diff[0b0100] = -1;
    rotate_diff[0b1101] = -1;
    rotate_diff[0b0010] = -1;
    rotate_diff[0b1011] = -1;

    // unused
    rotate_diff[0b0011] = 0;
    rotate_diff[0b0110] = 0;
    rotate_diff[0b1001] = 0;
    rotate_diff[0b1100] = 0;

    uint8_t a_prev = bcm2835_gpio_lev(ENCODER_A_PIN);
    uint8_t b_prev = bcm2835_gpio_lev(ENCODER_B_PIN);
    uint8_t a_next = a_prev;
    uint8_t b_next = b_prev;

    int screen=0;

    // Blink
    while (1)
    {
	// Read some data
	uint8_t button = (1-bcm2835_gpio_lev(BUTTON_PIN));
	printf("button: %s\n", button ? "ON" : "OFF");
	
	a_prev = a_next;
	b_prev = b_next;
	a_next = bcm2835_gpio_lev(ENCODER_A_PIN);
	b_next = bcm2835_gpio_lev(ENCODER_B_PIN);
	uint8_t array_index = (a_prev << 3) | (b_prev << 2) | (a_next << 1) | b_next;
	assert(array_index < 16);
	int rotation = rotate_diff[array_index];
	screen += rotation;
	printf("encoder: %d\n", screen);
	// wait a bit
    }

    bcm2835_close();
    return 0;
}

