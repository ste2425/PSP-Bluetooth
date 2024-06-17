const decoder = new TextDecoder();

document.addEventListener('DOMContentLoaded', () => {
  let writer;

  function log(text) {
    const el = document.createElement('p');

    el.textContent = text;

    output.appendChild(el);
  }

  const button = document.querySelector('#connect'),
    socket = new WebSocket(`wss://${location.host}/ws`),
    output = document.querySelector('#output');

  socket.binaryType = 'arraybuffer';

  socket.addEventListener('open', () => log("WS: Open"));
  socket.addEventListener('message', ({ data }) => {
    log(`WS: ${decoder.decode(data)}`);

    if (writer)
      writer.write(data);
  });

  button.addEventListener('click', async () => {
    const port = await navigator.serial.requestPort();

    await port.open({ baudRate: 115200 });

    log("Serial: Open");

    writer = port.writable.getWriter();

    for await (const chunk of port.readable) {
      log(`From ESP: ${decoder.decode(chunk)}`);
      socket.send(chunk);
    }
  });
});