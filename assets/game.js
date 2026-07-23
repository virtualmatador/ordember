var audios_ = [];
var countDown_ = null;
var longPress_ = null;
var longPressed_ = false;
const AudioContext = window.AudioContext || window.webkitAudioContext;
const audioCtx = new AudioContext();

window.addEventListener('DOMContentLoaded', function() {
  var banner = document.getElementById('banner');
  banner.addEventListener('pointerdown', startLongPress);
  banner.addEventListener('pointerup', finishLongPress);
  banner.addEventListener('pointercancel', cancelLongPress);
  banner.addEventListener('pointerleave', cancelLongPress);
  banner.addEventListener('contextmenu', function(event) {
    event.preventDefault();
  });
});

function setup(waves) {
  var loaded = 0;
  waves.forEach(function(id) {
    var request = new XMLHttpRequest();
    request.open(
        'GET', cross_asset_domain_ + 'wave/' + id + '.wav', cross_asset_async_);
    request.responseType = 'arraybuffer';
    request.onload = function() {
      var audioData = request.response;
      audioCtx.decodeAudioData(audioData, function(buffer) {
        audios_[id] = buffer;
        if (++loaded == waves.length) {
          setTimeout(CallHandler, 0, 'body', 'setup', '');
        }
      });
    };
    request.send();
  });
}

function setPieceSize(size) {
  vars = document.querySelector(':root');
  vars.style.setProperty('--piece-size', `${size}`);
}

function playAudio(id) {
  var source = audioCtx.createBufferSource();
  source.buffer = audios_[id];
  source.connect(audioCtx.destination);
  source.start(0);
}

function resize(size) {
  var pieces = document.getElementById('pieces');
  while (size < pieces.children.length) {
    pieces.lastElementChild.remove();
  }
  while (size > pieces.children.length) {
    let piece = document.createElement('span');
    piece.innerText = pieces.children.length + 1;
    piece.onclick =
        CallHandler.bind(null, 'piece', 'tap', `${pieces.children.length}`);
    pieces.appendChild(piece);
  }
}

function setPiece(index, state, x, y) {
  document.getElementById('pieces').children[index].style.left = `${x}%`;
  document.getElementById('pieces').children[index].style.top = `${y}%`;
  document.getElementById('pieces').children[index].setAttribute(
      'data-state', state);
}

function setProgress(level, score, lives) {
  document.getElementById('level').innerText = level;
  document.getElementById('score').innerText = score;

  var livesElement = document.getElementById('lives');
  livesElement.replaceChildren();
  for (var index = 0; index < lives; ++index) {
    livesElement.appendChild(document.createElement('span'));
  }
}

function countDown(duration, handler, command, info) {
  countDown_ = setTimeout(CallHandler, duration * 1000, handler, command, info);
}

function stop() {
  clearTimeout(countDown_);
  countDown_ = null;
  CallHandler('game', 'stop', '');
}

function go() {
  clearTimeout(countDown_);
  countDown_ = null;
  CallHandler('game', 'go', '');
}

function giveUp() {
  clearTimeout(countDown_);
  countDown_ = null;
  CallHandler('game', 'giveup', '');
}

function startLongPress(event) {
  if (event.pointerType != 'mouse' || event.button == 0) {
    longPressed_ = false;
    longPress_ = setTimeout(function() {
      longPressed_ = true;
      giveUp();
    }, 800);
  }
}

function finishLongPress() {
  clearTimeout(longPress_);
  longPress_ = null;
  if (!longPressed_) {
    go();
  }
}

function cancelLongPress() {
  clearTimeout(longPress_);
  longPress_ = null;
}

function banner(color, text) {
  document.getElementById('banner').style.backgroundColor = color;
  document.getElementById('banner').innerText = text;
}
