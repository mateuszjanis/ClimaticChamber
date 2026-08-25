#include "ChamberControl.h"

#define CHIP_PATH "/dev/gpiochip4" // RP1 na RPi 5
#define PIN_1 17
#define PIN_2 27
#define CONSUMER "chamber_rpi5"

int main() {
    try {
        // 1. Otwarcie układu GPIO
        auto chip = ::gpiod::chip(CHIP_PATH);

        ::gpiod::line::offsets offsets = {PIN_1, PIN_2};

        // 2. Konfiguracja i żądanie linii w stanie WYSOKIM (ACTIVE)
        auto request = chip.prepare_request()
            .set_consumer(CONSUMER)
            .add_line_settings(
                offsets,
                ::gpiod::line_settings()
                    .set_direction(::gpiod::line::direction::OUTPUT)
                    .set_output_value(::gpiod::line::value::ACTIVE)
            )
            .do_request();

        // 3. Opóźnienie 1 sekunda
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // 4. Zmiana stanu obu pinów na NISKI (INACTIVE)
        request.set_values({
            ::gpiod::line::value::INACTIVE,
            ::gpiod::line::value::INACTIVE
        });

    } catch (const std::exception& e) {
        std::cerr << "Błąd GPIO: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}