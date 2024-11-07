const pspButtons = {
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
    analogDown: 104
};

const controllerBits = {
    // when controller type is analog
    leftAnalog: 1,
    rightAnalog: 2,
    leftAnalogUp: 3,
    leftAnalogDown: 4,
    leftAnalogLeft: 5,
    leftAnalogRight: 6,
    rightAnalogUp: 7,
    rightAnalogDown: 8,
    rightAnalogLeft: 9,
    rightAnalogRight: 10,
    // when controller type is dpad
    dpadUp: 0,
    dpadDown: 1,
    dpadRight: 2,
    dpadLeft: 3,
    // when controller type is misc
    miscHome: 0,
    miscSelect: 1,
    miscStart: 2,
    miscCapture: 4,
    // when controller type button
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

const controllerTypes = {
    dpad: 0,
    misc: 1,
    button: 2,
    analog: 3
};

const mappings = [
    //default
    {
        "n": 1,
        "c": [90, 90, 255, 1],
        "m": [
            //dpad
            [pspButtons.up, controllerBits.dpadUp, controllerTypes.dpad],
            [pspButtons.down, controllerBits.dpadDown, controllerTypes.dpad],
            [pspButtons.left, controllerBits.dpadLeft, controllerTypes.dpad],
            [pspButtons.right, controllerBits.dpadRight, controllerTypes.dpad],
            //misc
            [pspButtons.home, controllerBits.miscHome, controllerTypes.misc],
            [pspButtons.select, controllerBits.miscSelect, controllerTypes.misc],
            [pspButtons.start, controllerBits.miscStart, controllerTypes.misc],
            //button
            [pspButtons.cross, controllerBits.crossButton, controllerTypes.button],            
            [pspButtons.circle, controllerBits.circleButton, controllerTypes.button],
            [pspButtons.triangle, controllerBits.triangleButton, controllerTypes.button],
            [pspButtons.square, controllerBits.squareButton, controllerTypes.button],
            [pspButtons.r1, controllerBits.r1, controllerTypes.button],
            [pspButtons.l1, controllerBits.l2, controllerTypes.button],
            //analog
            [0, controllerBits.leftAnalog, controllerTypes.analog]
        ]
    },
    //ps1
    {
        "n": 2,
        "c": [0, 250, 0, 1],
        "m": [
            //dpad
            [pspButtons.up, controllerBits.dpadUp, controllerTypes.dpad],
            [pspButtons.down, controllerBits.dpadDown, controllerTypes.dpad],
            [pspButtons.left, controllerBits.dpadLeft, controllerTypes.dpad],
            [pspButtons.right, controllerBits.dpadRight, controllerTypes.dpad],
            //misc
            [pspButtons.home, controllerBits.miscHome, controllerTypes.misc],
            [pspButtons.select, controllerBits.miscSelect, controllerTypes.misc],
            [pspButtons.start, controllerBits.miscStart, controllerTypes.misc],
            //button
            [pspButtons.cross, controllerBits.crossButton, controllerTypes.button],            
            [pspButtons.circle, controllerBits.circleButton, controllerTypes.button],
            [pspButtons.triangle, controllerBits.triangleButton, controllerTypes.button],
            [pspButtons.square, controllerBits.squareButton, controllerTypes.button],
            [pspButtons.r1, controllerBits.r1, controllerTypes.button],
            [pspButtons.l1, controllerBits.l2, controllerTypes.button],
            //analog
            [pspButtons.analogLeft, controllerBits.l2, controllerTypes.button],
            [pspButtons.analogRight, controllerBits.r2, controllerTypes.button]
        ]
    }
];

const json = `"${JSON.stringify(mappings).replace(/"/g, '\\"')}"`;

require('child_process').spawn('clip').stdin.end(json); 

/*
1:  0001
2:  0010
4:  0100
8:  1000
9:  1001
10: 1010
12: 1100
13: 1101
14: 1110
15: 1111

*/