function play()
{
    CallHandler("play", "click", "");
}

function reset()
{
    CallHandler("reset", "click", "");
}

function setSound(state)
{
    document.getElementById("sound").checked = state;
}

function sound()
{
    CallHandler("sound", "click", document.getElementById("sound").checked.toString());
}
