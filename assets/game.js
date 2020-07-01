var audios_= [];
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
                if (++loaded == ids.length)
                {
                    setTimeout(CallHandler, 0, 'body', 'setup', '');
                }
            });
        };
        request.send();
    });
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
            let piece = document.createElement('div');
            piece.innerText = document.getElementById("pieces").children.length + 1;
            document.getElementById("pieces").appendChild(piece);
        }
    }
}

function tap(index)
{
    CallHandler("game", "step", index.toString());
}

function setPiece(index, state, x, y)
{
    document.getElementById("pieces").children[index].style().x = x;
    document.getElementById("pieces").children[index].style().y = y;
    document.getElementById("pieces").children[index].setAttribute(
        "data-state", state);
}

function newGame()
{
    more();
    CallHandler("game", "reset", "");
}

function stop()
{
    more();
    CallHandler("game", "stop", "");
}

function giveUp()
{
    more();
    CallHandler("game", "giveup", "");
}

function gameOver(state)
{
    var banner = document.getElementById("gameover");
    banner.setAttribute("data-show", state);
    switch(state)
    {
        case 0:
            banner.textContent = "";
        break;
        case 1:
            banner.textContent = "You Win!";
        break;
        case 2:
            banner.textContent = "Game Over!";
        break;
    }
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
