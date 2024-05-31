#include "pspBluetoothApp.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// TODO - Move this
enum CTR_dpad {
  CTR_DPAD_UP = 1 << 0,
  CTR_DPAD_DOWN = 1 << 1,
  CTR_DPAD_RIGHT = 1 << 2,
  CTR_DPAD_LEFT = 1 << 3,
};

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void processGamepad(ControllerPtr ctl) {
    if (ctl->a())
        PSP_press_button(PSP_CROSS);

    if (ctl->b())
        PSP_press_button(PSP_SQUARE);

    if(ctl->x())
        PSP_press_button(PSP_CIRCLE);

    if(ctl->x())
        PSP_press_button(PSP_TRIANGLE);

    if(ctl->dpad() & CTR_DPAD_UP)
        PSP_press_button(PSP_DUP);

    if(ctl->dpad() & CTR_DPAD_DOWN)
        PSP_press_button(PSP_DDOWN);

    if(ctl->dpad() & CTR_DPAD_LEFT)
        PSP_press_button(PSP_DLEFT);

    if(ctl->dpad() & CTR_DPAD_RIGHT)
        PSP_press_button(PSP_DRIGHT);
}

void processControllers() {
    for (auto controller : myControllers) {
        if (controller && controller->isConnected() && controller->hasData() && controller->isGamepad()) {
            processGamepad(controller);
        }
    }
}

// Arduino setup function. Runs in CPU 1
void pspBluetooth_setup() {
    PSP_setup();

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But it might also fix some connection / re-connection issues.
    BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);
}

// Arduino loop function. Runs in CPU 1.
void pspBluetooth_loop() {
    PSP_mark_all_for_release();
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);

    PSP_release_unused();
}