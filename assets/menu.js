function play()
{
    CallHandler("play", "click", "");
}

function reset()
{
    CallHandler("reset", "click", "");
}

function setLives(size)
{
    if (size < document.getElementById("lives").children.length) {
        document.getElementById("lives").children.length = size;
    } else {
        while (size > document.getElementById("lives").children.length) {
            document.getElementById("lives").appendChild(document.createElement('div'));
        }
    }
}

function setLevel(level)
{
    document.getElementById("level").innerText = level;
}

function setScore(score)
{
    document.getElementById("score").innerText = score;
}

function setSound(state)
{
    document.getElementById("sound").checked = state;
}

function sound()
{
    CallHandler("sound", "click", document.getElementById("sound").checked.toString());
}
