var audios_= [];
var countDown_ = null;
const AudioContext = window.AudioContext || window.webkitAudioContext;
const audioCtx = new AudioContext();

function setup(waves)
{
    var loaded = 0;
    waves.forEach(function(id)
    {
        var request = new XMLHttpRequest();
        request.open('GET', cross_asset_domain_ + 'wave/' + id + '.wav', cross_asset_async_);
        request.responseType = 'arraybuffer';
        request.onload = function()
        {
            var audioData = request.response;
            audioCtx.decodeAudioData(audioData, function(buffer)
            {
                audios_[id] = buffer;
                if (++loaded == waves.length)
                {
                    setTimeout(CallHandler, 0, 'body', 'setup', '');
                }
            });
        };
        request.send();
    });
}

function setPieceSize(size)
{
    vars = document.querySelector(":root");
    vars.style.setProperty("--piece-size", `${size}`);
}

function playAudio(id)
{
    var source = audioCtx.createBufferSource();
    source.buffer = audios_[id];
    source.connect(audioCtx.destination);
    source.start(0);
}

function resize(size) {
    if (size < document.getElementById("pieces").children.length) {
        document.getElementById("pieces").children.length = size;
    } else {
        while (size > document.getElementById("pieces").children.length) {
            let piece = document.createElement('span');
            piece.innerText = document.getElementById("pieces").children.length + 1;
            piece.onclick = CallHandler.bind(
                null, "piece", "tap", `${document.getElementById("pieces").children.length}`);
            document.getElementById("pieces").appendChild(piece);
        }
    }
}

function setPiece(index, state, x, y)
{
    document.getElementById("pieces").children[index].style.left = `${x}%`;
    document.getElementById("pieces").children[index].style.top = `${y}%`;
    document.getElementById("pieces").children[index].setAttribute(
        "data-state", state);
}

function countDown(duration, handler, command, info)
{
    countDown_ = setTimeout(CallHandler, duration * 1000, handler, command, info);
}

function stop()
{
    clearTimeout(countDown_);
    countDown_ = null;
    more();
    CallHandler("game", "stop", "");
}

function go(hide)
{
    clearTimeout(countDown_);
    countDown_ = null;
    if (hide)
    {
        more();
    }
    CallHandler("game", "go", "");
}

function giveUp()
{
    clearTimeout(countDown_);
    countDown_ = null;
    more();
    CallHandler("game", "giveup", "");
}

function banner(color, text)
{
    document.getElementById("banner").style.backgroundColor = color;
    document.getElementById("banner").innerText = text;
}

function more()
{
    var options = document.getElementById("options");
    if (options.style.display == "block")
    {
        options.style.display = "none";
    }
    else
    {
        options.style.display = "block";
    }
}
