#include "pspBluetoothApp.h"

static ControllerPtr myControllers[BP32_MAX_GAMEPADS];
static uint8_t mappingLength = 10;

enum CTR_accessor_type {
    CTR_Accessor_Button = 1,
    CTR_Accessor_Dpad = 2,
};
uint8_t mappings[30][3] = {
    {
        DPAD_UP, CTR_Accessor_Dpad, 5
    },
    {
        DPAD_DOWN, CTR_Accessor_Dpad, 6
    },
    {
        DPAD_RIGHT, CTR_Accessor_Dpad, 7
    },
    {
        DPAD_LEFT, CTR_Accessor_Dpad, 8
    },
    {
        BUTTON_A, CTR_Accessor_Button, 10
    },
    {
        BUTTON_B, CTR_Accessor_Button, 11
    },
    {
        BUTTON_X, CTR_Accessor_Button, 12
    },
    {
        BUTTON_Y, CTR_Accessor_Button, 13
    },
    {
        BUTTON_SHOULDER_L, CTR_Accessor_Button, 4
    },
    {
        BUTTON_SHOULDER_R, CTR_Accessor_Button, 9
    }
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
    bool allEmpty = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }

        if (myControllers[i] != nullptr)
            allEmpty = false;
    }

    if (allEmpty) {
        Serial.println("No more controllers connected, released all PSP buttons");
        
        PSP_mark_all_for_release();
        PSP_release_unused();
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void processGamepad(ControllerPtr ctl) {
    for (uint8_t i = 0; i < mappingLength; i++) {
        uint8_t mask = mappings[i][0],
            accessor = mappings[i][1],
            pspPin = mappings[i][2];

        uint8_t pressed = 0;

        switch (accessor) {
            case CTR_Accessor_Dpad:

            if (ctl->dpad() & mask)
                PSP_press_button(pspPin);

            break;
            case CTR_Accessor_Button:

            if (ctl->buttons() & mask)
                PSP_press_button(pspPin);

            break;
        }
    }

    /*if (ctl->l1())
        PSP_press_button(PSP_L1);

    if(ctl->r1())
        PSP_press_button(PSP_R1);
    
    if (ctl->a())
        PSP_press_button(PSP_CROSS);

    if (ctl->b())
        PSP_press_button(PSP_CIRCLE);

    if(ctl->x())
        PSP_press_button(PSP_SQUARE);

    if(ctl->x())
        PSP_press_button(PSP_TRIANGLE);

    if(ctl->dpad() & CTR_DPAD_UP)
        PSP_press_button(PSP_DUP);

    if(ctl->dpad() & CTR_DPAD_DOWN)
        PSP_press_button(PSP_DDOWN);

    if(ctl->dpad() & CTR_DPAD_LEFT)
        PSP_press_button(PSP_DLEFT);

    if(ctl->dpad() & CTR_DPAD_RIGHT)
        PSP_press_button(PSP_DRIGHT);*/
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

    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.forgetBluetoothKeys();

    BP32.enableVirtualDevice(false);
}

void pspBluetooth_loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated)
    {
        PSP_mark_all_for_release();
        processControllers();
        PSP_release_unused();
    }
}