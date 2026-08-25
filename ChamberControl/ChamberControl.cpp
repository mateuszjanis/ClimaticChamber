#include "ChamberControl.h"

#define CHIP_NAME "/dev/gpiochip4" // RP1 na RPi 5
#define PIN_1 17
#define PIN_2 27
#define CONSUMER "chamber_rpi5"

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line *line1, *line2;
    struct gpiod_line_bulk bulk;

    // 1. Otwarcie układu GPIO
    chip = gpiod_chip_open(CHIP_NAME);
    if (!chip) {
        perror("Błąd: gpiod_chip_open");
        return EXIT_FAILURE;
    }

    // 2. Pobranie referencji do linii
    line1 = gpiod_chip_get_line(chip, PIN_1);
    line2 = gpiod_chip_get_line(chip, PIN_2);

    if (!line1 || !line2) {
        perror("Błąd: gpiod_chip_get_line");
        gpiod_chip_close(chip);
        return EXIT_FAILURE;
    }

    // Inicjalizacja operacji masowej (jednoczesne przełączanie)
    gpiod_line_bulk_init(&bulk);
    gpiod_line_bulk_add_line(&bulk, line1);
    gpiod_line_bulk_add_line(&bulk, line2);

    // 3. Konfiguracja jako wyjścia ze stanem wysokim (1)
    int initial_values[2] = {1, 1};
    if (gpiod_line_request_bulk_output(&bulk, CONSUMER, initial_values) < 0) {
        perror("Błąd: gpiod_line_request_bulk_output");
        gpiod_chip_close(chip);
        return EXIT_FAILURE;
    }

    // 4. Opóźnienie 1 sekunda
    sleep(1);

    // 5. Zmiana na stan niski (0)
    int new_values[2] = {0, 0};
    if (gpiod_line_set_value_bulk(&bulk, new_values) < 0) {
        perror("Błąd: gpiod_line_set_value_bulk");
    }

    // 6. Zwolnienie zasobów
    gpiod_line_release_bulk(&bulk);
    gpiod_chip_close(chip);

    return EXIT_SUCCESS;
}
