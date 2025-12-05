var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (g && (g = 0, op[0] && (_ = 0)), _) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
var _this = this;
// @ts-nocheck
var NUM_LEDS = 50;
var NETWORK_ID = 0xcafebe16;
var BROADCAST_CHANNEL = 0xff;
// Binary:
// 11001010
// 11111110
// 10111110
// 00010110
var MessageType;
(function (MessageType) {
    MessageType[MessageType["ORDER"] = 0] = "ORDER";
    MessageType[MessageType["CONFIG"] = 1] = "CONFIG";
    MessageType[MessageType["COLORS24"] = 2] = "COLORS24";
    MessageType[MessageType["COLORS16"] = 3] = "COLORS16";
    MessageType[MessageType["COLORS8"] = 4] = "COLORS8";
})(MessageType || (MessageType = {}));
var CRGB = /** @class */ (function () {
    function CRGB(red, green, blue) {
        this.red = 0;
        this.green = 0;
        this.blue = 0;
        this.red = red;
        this.green = green;
        this.blue = blue;
    }
    Object.defineProperty(CRGB.prototype, "RGB24", {
        get: function () {
            return new Uint8Array([this.red, this.green, this.blue]);
        },
        enumerable: false,
        configurable: true
    });
    Object.defineProperty(CRGB.prototype, "RGB16", {
        get: function () {
            var r = (this.red >> 3) & 0x1f;
            var g = (this.green >> 2) & 0x3f;
            var b = (this.blue >> 3) & 0x1f;
            var value = (r << 11) | (g << 5) | b;
            return new Uint8Array([value & 0xff, value >> 8]);
        },
        enumerable: false,
        configurable: true
    });
    Object.defineProperty(CRGB.prototype, "RGB8", {
        get: function () {
            var r = (this.red >> 5) & 0x07;
            var g = (this.green >> 5) & 0x07;
            var b = (this.blue >> 6) & 0x03;
            return new Uint8Array([(r << 5) | (g << 2) | b]);
        },
        enumerable: false,
        configurable: true
    });
    return CRGB;
}());
function offsetOf(field) {
    var sizes = {
        networkId: 4,
        length: 1,
        order: 2,
        type: 1,
        channels: 1,
    };
    var offset = 0;
    for (var _i = 0, _a = Object.entries(sizes); _i < _a.length; _i++) {
        var _b = _a[_i], name_1 = _b[0], size = _b[1];
        if (name_1 === field)
            break;
        offset += size;
    }
    return offset;
}
var Message = /** @class */ (function () {
    function Message() {
        this.binary = new Uint8Array(212);
        this.channelsColors = {};
        this.networkId = new Uint32Array(1);
        this.length = new Uint8Array(1);
        this.order = new Uint16Array(1);
        this.type = new Uint8Array(1);
        this.channels = new Uint8Array(1);
    }
    Message.prototype.setNetworkId = function (networkId) {
        this.networkId[0] = networkId;
    };
    Message.prototype.setOrder = function (order) {
        this.order[0] = order;
    };
    Message.prototype.setType = function (type) {
        this.type[0] = type;
    };
    Message.prototype.setRGB24 = function (channelId, colors) {
        if (this.type[0] !== MessageType.COLORS24)
            throw new Error("Message type is not COLORS24");
        this.channels[0] |= channelId === BROADCAST_CHANNEL ? BROADCAST_CHANNEL : 1 << channelId;
        this.channelsColors[channelId] = new Uint8Array(NUM_LEDS * 3);
        for (var i = 0; i < NUM_LEDS; i++)
            this.channelsColors[channelId].set(colors[i].RGB24, i * 3);
    };
    Message.prototype.setRGB16 = function (channelId, colors) {
        if (this.type[0] !== MessageType.COLORS16)
            throw new Error("Message type is not COLORS16");
        this.channels[0] |= channelId === BROADCAST_CHANNEL ? BROADCAST_CHANNEL : 1 << channelId;
        this.channelsColors[channelId] = new Uint8Array(NUM_LEDS * 2);
        for (var i = 0; i < NUM_LEDS; i++)
            this.channelsColors[channelId].set(colors[i].RGB16, i * 2);
    };
    Message.prototype.setRGB8 = function (channelId, colors) {
        if (this.type[0] !== MessageType.COLORS8)
            throw new Error("Message type is not COLORS8");
        this.channels[0] |= channelId === BROADCAST_CHANNEL ? BROADCAST_CHANNEL : 1 << channelId;
        this.channelsColors[channelId] = new Uint8Array(NUM_LEDS);
        for (var i = 0; i < NUM_LEDS; i++)
            this.channelsColors[channelId].set(colors[i].RGB8, i);
    };
    Message.prototype.getBinary = function () {
        this.binary.set(new Uint8Array(this.networkId.buffer), offsetOf("networkId"));
        this.binary.set(new Uint8Array(this.order.buffer), offsetOf("order"));
        this.binary.set(new Uint8Array(this.type.buffer), offsetOf("type"));
        this.binary.set(new Uint8Array(this.channels.buffer), offsetOf("channels"));
        if (this.type[0] === MessageType.ORDER || this.type[0] === MessageType.CONFIG)
            return this.binary;
        var offset = offsetOf("padding") + 3;
        var mc = 0; // max channels
        var bpl = 0; // bytes per LED
        this.type[0] === MessageType.COLORS24 && (bpl = 3) && (mc = 1);
        this.type[0] === MessageType.COLORS16 && (bpl = 2) && (mc = 2);
        this.type[0] === MessageType.COLORS8 && (bpl = 1) && (mc = 4);
        if (BROADCAST_CHANNEL in this.channelsColors) {
            this.channels[0] = BROADCAST_CHANNEL;
            this.binary.set(this.channels, offset);
            this.binary.set(this.channelsColors[BROADCAST_CHANNEL], offset);
            this.binary = this.binary.slice(0, offset + NUM_LEDS * bpl);
        }
        else {
            // Count channels
            var channels = [];
            for (var i = 0; i < 8; i++)
                if (this.channels[0] & (1 << i))
                    channels.push(i);
            if (channels.length === 0)
                throw new Error("No channels set in the message");
            if (channels.length > mc)
                throw new Error("Too many channels set in the message ".concat(MessageType[this.type[0]]));
            // Trim binary to used size
            this.binary = this.binary.slice(0, offset + NUM_LEDS * bpl * channels.length);
            // Fill colors
            var payloadId = 0; // payload index
            for (var _i = 0, channels_1 = channels; _i < channels_1.length; _i++) {
                var i = channels_1[_i];
                if (this.channelsColors[i] === undefined)
                    throw new Error("Channel ".concat(i, " colors are not set"));
                var channelOffset = NUM_LEDS * bpl * payloadId++;
                this.binary.set(this.channelsColors[i], offset + channelOffset);
            }
        }
        this.length[0] = this.binary.byteLength;
        this.binary.set(new Uint8Array(this.length.buffer), offsetOf("length"));
        return this.binary;
    };
    return Message;
}());
// const buttonConnect = document.getElementById("connect");
// const buttonSend = document.getElementById("send");
function log(bytes, format, reverse, size) {
    if (format === void 0) { format = "hex"; }
    if (reverse === void 0) { reverse = true; }
    if (size === void 0) { size = true; }
    var output = "";
    if (reverse)
        bytes = bytes.reverse();
    for (var i = 0; i < bytes.length; i++) {
        if (format === "bin")
            output += "0b" + bytes[i].toString(2).padStart(8, "0") + " ";
        if (format === "hex")
            output += "0x" + bytes[i].toString(16).padStart(2, "0").toUpperCase() + " ";
    }
    return output.trim() + (reverse ? " (r)" : "") + (size ? " | ".concat(bytes.length, "B") : "");
}
var message = new Message();
message.setNetworkId(NETWORK_ID);
message.setOrder(1);
message.setType(MessageType.COLORS16);
var allRed = new Array(NUM_LEDS).fill(new CRGB(255, 0, 0));
var allGreen = new Array(NUM_LEDS).fill(new CRGB(0, 255, 0));
message.setRGB16(0, allRed);
message.setRGB16(1, allGreen);
var binary = message.getBinary();
console.log("Message binary:");
console.log("Network ID:".padStart(12), log(binary.slice(0, 4), "hex").padStart(40));
console.log("Length:".padStart(12), log(binary.slice(4, 5), "hex").padStart(40));
console.log("Order:".padStart(12), log(binary.slice(5, 7), "hex").padStart(40));
console.log("Type:".padStart(12), log(binary.slice(7, 8), "hex").padStart(40));
console.log("Channels:".padStart(12), log(binary.slice(8, 9), "bin").padStart(40));
console.log("Colors :".padStart(12));
var colorBytes = 0;
if (binary[7] === MessageType.COLORS24)
    colorBytes = 3;
if (binary[7] === MessageType.COLORS16)
    colorBytes = 2;
if (binary[7] === MessageType.COLORS8)
    colorBytes = 1;
var channelCount = 0;
if (binary[8] === BROADCAST_CHANNEL)
    channelCount = 1;
else {
    for (var i = 0; i < 8; i++)
        if (binary[8] & (1 << i))
            channelCount++;
}
var inRow = 10;
for (var c = 0; c < channelCount; c++) {
    console.log("".padStart(12), "Channel ".concat(c, ":"));
    for (var i = 0; i < NUM_LEDS; i += inRow) {
        var start = 9 + c * NUM_LEDS * colorBytes + i * colorBytes;
        var end = Math.min(start + inRow * colorBytes, 9 + (c + 1) * NUM_LEDS * colorBytes);
        console.log("".padStart(12), log(binary.slice(start, end)).padStart(40));
    }
}
console.log("Total size:".padStart(12), binary.length, "bytes");
var port;
var writer;
var reader;
document.getElementById("connect").onclick = function () { return __awaiter(_this, void 0, void 0, function () {
    var readInt;
    var _this = this;
    return __generator(this, function (_a) {
        switch (_a.label) {
            case 0: return [4 /*yield*/, navigator.serial.requestPort()];
            case 1:
                port = _a.sent();
                return [4 /*yield*/, port.open({ baudRate: 115200 })];
            case 2:
                _a.sent();
                writer = port.writable.getWriter();
                reader = port.readable.getReader();
                readInt = setInterval(function () { return __awaiter(_this, void 0, void 0, function () {
                    var _a, value, done, decoded, error_1;
                    return __generator(this, function (_b) {
                        switch (_b.label) {
                            case 0:
                                _b.trys.push([0, 2, , 3]);
                                return [4 /*yield*/, reader.read()];
                            case 1:
                                _a = _b.sent(), value = _a.value, done = _a.done;
                                if (done) {
                                    console.log("Reader closed");
                                    clearInterval(readInt);
                                    return [2 /*return*/];
                                }
                                decoded = new TextDecoder().decode(value);
                                console.log("Received:", decoded);
                                return [3 /*break*/, 3];
                            case 2:
                                error_1 = _b.sent();
                                console.error("Read error:", error_1);
                                clearInterval(readInt);
                                return [3 /*break*/, 3];
                            case 3: return [2 /*return*/];
                        }
                    });
                }); }, 100);
                return [2 /*return*/];
        }
    });
}); };
document.getElementById("send1").onclick = function () { return __awaiter(_this, void 0, void 0, function () {
    return __generator(this, function (_a) {
        switch (_a.label) {
            case 0: return [4 /*yield*/, writer.write(binary)];
            case 1:
                _a.sent();
                return [2 /*return*/];
        }
    });
}); };
document.getElementById("sendLong").onclick = function () { return __awaiter(_this, void 0, void 0, function () {
    var it;
    var _this = this;
    return __generator(this, function (_a) {
        it = setInterval(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, writer.write(binary)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        setTimeout(function () {
            clearInterval(it);
        }, 10000);
        return [2 /*return*/];
    });
}); };
