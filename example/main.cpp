// Other includes
#include <bridge.h>

// Standard includes
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

void
perform_gpio_test(Brg *stlink_bridge);

int
main()
{
    cout << "Welcome to the example!" << endl;

    STLinkInterface stlink_interface;
    Brg stlink_bridge(stlink_interface);

#ifdef USING_ERRORLOG
    cErrLog err_log;
    stlink_interface.BindErrLog(&err_log);
    stlink_bridge.BindErrLog(&err_log);
#endif

    cout << "Attempting to open ST-Link... ";
    stlink_interface.LoadStlinkLibrary("");
    Brg_StatusT status = stlink_bridge.OpenStlink();
    switch (status) {
    case BRG_NO_ERR:
        cout << "success!" << endl;
        break;
    default:
        cout << "fail! Some error occured (" << status << ")" << endl;
        break;
    }

    if (status == BRG_NO_ERR) {
        perform_gpio_test(&stlink_bridge);
        return 0;
    } else {
        return -1;
    }
}

int
brg_status_print(Brg_StatusT status)
{
    switch (status) {
    case BRG_NO_ERR:
        cout << "success!" << endl;
        return 0;
    default:
        cout << "fail! Some error occured (" << status << ")" << endl;
        return 1;
    }
}

// set gpio level
void
gpio_set_level(Brg *brg, int channel, Brg_GpioValT level)
{
    Brg_StatusT status;
    uint8_t error_mask = 0;
    Brg_GpioValT gpio_vals[BRG_GPIO_MAX_NB];

    gpio_vals[channel] = level ? GPIO_SET : GPIO_RESET;

    status = brg->SetResetGPIO(BRG_GPIO_ALL, gpio_vals, &error_mask);
}

void
perform_gpio_test(Brg *stlink_bridge)
{
    Brg_StatusT status;
    // Define just one to apply to all GPIOs
    Brg_GpioConfT gpio_confs[] = {
        {
            .Mode = GPIO_MODE_OUTPUT,
            .Speed = GPIO_SPEED_LOW,
            .Pull = GPIO_PULL_UP,
            .OutputType = GPIO_OUTPUT_PUSHPULL,
        },
    };

    Brg_GpioInitT init = {
        .GpioMask = BRG_GPIO_ALL,
        .ConfigNb = ARRAY_SIZE(gpio_confs),
        .pGpioConf = gpio_confs,
    };

    cout << "Attempting to configure GPIOs... ";
    status = stlink_bridge->InitGPIO(&init);
    if (brg_status_print(status)) {
        return;
    }

    // initial state
    gpio_set_level(stlink_bridge, 0, GPIO_RESET);
    gpio_set_level(stlink_bridge, 1, GPIO_RESET);
    gpio_set_level(stlink_bridge, 2, GPIO_RESET);
    gpio_set_level(stlink_bridge, 3, GPIO_RESET);

    while (1) {
        gpio_set_level(stlink_bridge, 0, GPIO_SET);
        gpio_set_level(stlink_bridge, 1, GPIO_SET);
        gpio_set_level(stlink_bridge, 2, GPIO_SET);
        gpio_set_level(stlink_bridge, 3, GPIO_SET);
    }
}
