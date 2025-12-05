// @ts-nocheck
const NUM_LEDS = 50;
const NETWORK_ID = 0xcafebe16;
const BROADCAST_CHANNEL = 0xff;
// Binary:
// 11001010
// 11111110
// 10111110
// 00010110

enum MessageType {
  ORDER,
  CONFIG,
  COLORS24,
  COLORS16,
  COLORS8,
}

class CRGB {
  public red: number = 0;
  public green: number = 0;
  public blue: number = 0;

  constructor(red: number, green: number, blue: number) {
    this.red = red;
    this.green = green;
    this.blue = blue;
  }

  get RGB24(): Uint8Array {
    return new Uint8Array([this.red, this.green, this.blue]);
  }

  get RGB16(): Uint8Array {
    const r = (this.red >> 3) & 0x1f;
    const g = (this.green >> 2) & 0x3f;
    const b = (this.blue >> 3) & 0x1f;
    const value = (r << 11) | (g << 5) | b;
    return new Uint8Array([value & 0xff, value >> 8]);
  }

  get RGB8(): Uint8Array {
    const r = (this.red >> 5) & 0x07;
    const g = (this.green >> 5) & 0x07;
    const b = (this.blue >> 6) & 0x03;
    return new Uint8Array([(r << 5) | (g << 2) | b]);
  }
}

type Header = {
  networkId: number;
  order: number;
  type: MessageType;
  channels: number[];
};

function offsetOf(field: keyof Header) {
  const sizes: Record<keyof Header, number> = {
    networkId: 4,
    length: 1,
    order: 2,
    type: 1,
    channels: 1,
  };

  let offset = 0;
  for (const [name, size] of Object.entries(sizes)) {
    if (name === field) break;
    offset += size;
  }

  return offset;
}

type RGB24Channels = 0 | typeof BROADCAST_CHANNEL;
type RGB16Channels = 0 | 1 | typeof BROADCAST_CHANNEL;
type RGB8Channels = 0 | 1 | 2 | 3 | typeof BROADCAST_CHANNEL;

type ChannelsColors<T extends RGB24Channels | RGB16Channels | RGB8Channels> = {
  [key in T]?: Uint8Array;
};

class Message {
  private binary = new Uint8Array(212);

  private channelsColors: ChannelsColors<RGB24Channels | RGB16Channels | RGB8Channels> = {};

  private networkId = new Uint32Array(1);
  private length = new Uint8Array(1);
  private order = new Uint16Array(1);
  private type = new Uint8Array(1);
  private channels = new Uint8Array(1);

  constructor() {}

  public setNetworkId(networkId: number) {
    this.networkId[0] = networkId;
  }

  public setOrder(order: number) {
    this.order[0] = order;
  }

  public setType(type: MessageType) {
    this.type[0] = type;
  }

  public setRGB24(channelId: RGB24Channels, colors: CRGB[]) {
    if (this.type[0] !== MessageType.COLORS24) throw new Error("Message type is not COLORS24");

    this.channels[0] |= channelId === BROADCAST_CHANNEL ? BROADCAST_CHANNEL : 1 << channelId;

    this.channelsColors[channelId] = new Uint8Array(NUM_LEDS * 3);
    for (let i = 0; i < NUM_LEDS; i++) this.channelsColors[channelId].set(colors[i].RGB24, i * 3);
  }

  public setRGB16(channelId: RGB16Channels, colors: CRGB[]) {
    if (this.type[0] !== MessageType.COLORS16) throw new Error("Message type is not COLORS16");

    this.channels[0] |= channelId === BROADCAST_CHANNEL ? BROADCAST_CHANNEL : 1 << channelId;

    this.channelsColors[channelId] = new Uint8Array(NUM_LEDS * 2);
    for (let i = 0; i < NUM_LEDS; i++) this.channelsColors[channelId].set(colors[i].RGB16, i * 2);
  }

  public setRGB8(channelId: RGB8Channels, colors: CRGB[]) {
    if (this.type[0] !== MessageType.COLORS8) throw new Error("Message type is not COLORS8");

    this.channels[0] |= channelId === BROADCAST_CHANNEL ? BROADCAST_CHANNEL : 1 << channelId;

    this.channelsColors[channelId] = new Uint8Array(NUM_LEDS);
    for (let i = 0; i < NUM_LEDS; i++) this.channelsColors[channelId].set(colors[i].RGB8, i);
  }

  public getBinary(): Uint8Array {
    this.binary.set(new Uint8Array(this.networkId.buffer), offsetOf("networkId"));
    this.binary.set(new Uint8Array(this.order.buffer), offsetOf("order"));
    this.binary.set(new Uint8Array(this.type.buffer), offsetOf("type"));
    this.binary.set(new Uint8Array(this.channels.buffer), offsetOf("channels"));

    if (this.type[0] === MessageType.ORDER || this.type[0] === MessageType.CONFIG)
      return this.binary;

    const offset = offsetOf("channels") + 1;

    let mc = 0; // max channels
    let bpl = 0; // bytes per LED
    this.type[0] === MessageType.COLORS24 && (bpl = 3) && (mc = 1);
    this.type[0] === MessageType.COLORS16 && (bpl = 2) && (mc = 2);
    this.type[0] === MessageType.COLORS8 && (bpl = 1) && (mc = 4);

    if (BROADCAST_CHANNEL in this.channelsColors) {
      this.channels[0] = BROADCAST_CHANNEL;
      this.binary.set(this.channels, offset);
      this.binary.set(this.channelsColors[BROADCAST_CHANNEL]!, offset);
      this.binary = this.binary.slice(0, offset + NUM_LEDS * bpl);
    } else {
      // Count channels
      let channels: RGB24Channels[] = [];
      for (let i = 0; i < 8; i++)
        if (this.channels[0] & (1 << i)) channels.push(i as RGB24Channels);

      if (channels.length === 0) throw new Error("No channels set in the message");

      if (channels.length > mc)
        throw new Error(`Too many channels set in the message ${MessageType[this.type[0]]}`);

      // Trim binary to used size
      this.binary = this.binary.slice(0, offset + NUM_LEDS * bpl * channels.length);

      // Fill colors
      let payloadId = 0; // payload index
      for (const i of channels) {
        if (this.channelsColors[i] === undefined)
          throw new Error(`Channel ${i} colors are not set`);

        const channelOffset = NUM_LEDS * bpl * payloadId++;
        this.binary.set(this.channelsColors[i], offset + channelOffset);
      }
    }

    this.length[0] = this.binary.byteLength;
    this.binary.set(new Uint8Array(this.length.buffer), offsetOf("length"));
    return this.binary;
  }
}

// const buttonConnect = document.getElementById("connect");
// const buttonSend = document.getElementById("send");

function log(
  bytes: Uint8Array,
  format: "bin" | "hex" = "hex",
  reverse: boolean = true,
  size: boolean = true
): string {
  let output = "";
  if (reverse) bytes = bytes.reverse();
  for (let i = 0; i < bytes.length; i++) {
    if (format === "bin") output += "0b" + bytes[i].toString(2).padStart(8, "0") + " ";

    if (format === "hex")
      output += "0x" + bytes[i].toString(16).padStart(2, "0").toUpperCase() + " ";
  }
  return output.trim() + (reverse ? " (r)" : "") + (size ? ` | ${bytes.length}B` : "");
}

const message = new Message();
message.setNetworkId(NETWORK_ID);
message.setOrder(1);
message.setType(MessageType.COLORS16);

const allRed: CRGB[] = new Array<CRGB>(NUM_LEDS).fill(new CRGB(255, 0, 0));
const allGreen: CRGB[] = new Array<CRGB>(NUM_LEDS).fill(new CRGB(0, 255, 0));

message.setRGB16(0, allRed);
message.setRGB16(1, allGreen);

const binary = message.getBinary();

console.log("Message binary:");
console.log("Network ID:".padStart(12), log(binary.slice(0, 4), "hex").padStart(40));
console.log("Length:".padStart(12), log(binary.slice(4, 5), "hex").padStart(40));
console.log("Order:".padStart(12), log(binary.slice(5, 7), "hex").padStart(40));
console.log("Type:".padStart(12), log(binary.slice(7, 8), "hex").padStart(40));
console.log("Channels:".padStart(12), log(binary.slice(8, 9), "bin").padStart(40));
console.log("Colors :".padStart(12));

let colorBytes = 0;
if (binary[7] === MessageType.COLORS24) colorBytes = 3;
if (binary[7] === MessageType.COLORS16) colorBytes = 2;
if (binary[7] === MessageType.COLORS8) colorBytes = 1;

let channelCount = 0;
if (binary[8] === BROADCAST_CHANNEL) channelCount = 1;
else {
  for (let i = 0; i < 8; i++) if (binary[8] & (1 << i)) channelCount++;
}

const inRow = 10;
for (let c = 0; c < channelCount; c++) {
  console.log("".padStart(12), `Channel ${c}:`);
  for (let i = 0; i < NUM_LEDS; i += inRow) {
    const start = 9 + c * NUM_LEDS * colorBytes + i * colorBytes;
    const end = Math.min(start + inRow * colorBytes, 9 + (c + 1) * NUM_LEDS * colorBytes);

    console.log("".padStart(12), log(binary.slice(start, end)).padStart(40));
  }
}

console.log("Total size:".padStart(12), binary.length, "bytes");

let port: SerialPort;
let writer: WritableStreamDefaultWriter<Uint8Array>;
let reader: ReadableStreamDefaultReader<Uint8Array>;

document.getElementById("connect").onclick = async () => {
  port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200 });
  writer = port.writable.getWriter();
  reader = port.readable.getReader();

  const readInt = setInterval(async () => {
    try {
      const { value, done } = await reader.read();
      if (done) {
        console.log("Reader closed");
        clearInterval(readInt);
        return;
      }
      const decoded = new TextDecoder().decode(value);
      console.log("Received:", decoded);
    } catch (error) {
      console.error("Read error:", error);
      clearInterval(readInt);
    }
  }, 100);
};

document.getElementById("send1").onclick = async () => {
  await writer.write(binary);
};

document.getElementById("sendLong").onclick = async () => {
  const it = setInterval(async () => {
    await writer.write(binary);
  });

  setTimeout(() => {
    clearInterval(it);
  }, 10_000);
};
