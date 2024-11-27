export const pspButtons = {
    empty: 99,
    right: 0,
    circle: 1,
    cross: 2,
    power: 3,
    triangle: 4,
    r1: 5,
    square: 6,
    l1: 7,
    up: 8,
    left: 9,
    down: 10,
    sound: 11,
    display: 12,
    volup: 13,
    voldown: 14,
    home: 15,
    start: 16,
    select: 17,
    analogRight: 101,
    analogLeft: 102,
    analogUp: 103,
    analogDown: 104,
    analog: 105
};

export const availablePspButtons = Object.values(pspButtons);

// when controller type is analog
export const analogControllerBits = {
    leftAnalog: 1,
    rightAnalog: 2,
    leftAnalogUp: 3,
    leftAnalogDown: 4,
    leftAnalogLeft: 5,
    leftAnalogRight: 6,
    rightAnalogUp: 7,
    rightAnalogDown: 8,
    rightAnalogLeft: 9,
    rightAnalogRight: 10
};

// when controller type is dpad
export const dpadControllerBits = {
    dpadUp: 0,
    dpadDown: 1,
    dpadRight: 2,
    dpadLeft: 3
};

// when controller type is misc
export const miscControllerBits = {
    miscHome: 0,
    miscSelect: 1,
    miscStart: 2,
    miscCapture: 4
};

// when controller type button
export const buttonControllerBits = {
    crossButton: 0,
    circleButton: 1,
    squareButton: 2,
    triangleButton: 3,
    l1: 4,
    l2: 6,
    r1: 5,
    r2: 7,
    thumbL: 8,
    thumbR: 9
};

export const controllerTypes = {
    dpad: 0,
    misc: 1,
    button: 2,
    analog: 3
};
export const availablecontrollerTypes = Object.values(controllerTypes);

export const controllerBitsMappedToType = {
    [controllerTypes.dpad]: dpadControllerBits,
    [controllerTypes.misc]: miscControllerBits,
    [controllerTypes.button]: buttonControllerBits,
    [controllerTypes.analog]: analogControllerBits
};

export const availableControllerBitsMappedToType = {
    [controllerTypes.dpad]: Object.values(dpadControllerBits),
    [controllerTypes.misc]: Object.values(miscControllerBits),
    [controllerTypes.button]: Object.values(buttonControllerBits),
    [controllerTypes.analog]: Object.values(analogControllerBits)
};