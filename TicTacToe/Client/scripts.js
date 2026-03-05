
const create = document.getElementById("create");
const join = document.getElementById("join");
const connectstatus = document.getElementById("status");
const matchid = document.getElementById("matchid");
const mid = document.getElementById("mid");
const movestatus = document.getElementById("movestatus");

const matrix = [
    [
        document.getElementById("a0b0"),
        document.getElementById("a0b1"),
        document.getElementById("a0b2")
    ],
    [
        document.getElementById("a1b0"),
        document.getElementById("a1b1"),
        document.getElementById("a1b2")
    ],
    [
        document.getElementById("a2b0"),
        document.getElementById("a2b1"),
        document.getElementById("a2b2")
    ]
];



let player, match_id;



const socket = new WebSocket("ws://localhost:8080")

socket.onopen = () => {
    console.log("Connected to game server");
    connectstatus.textContent = "Connection Status: Connected"
    create.disabled = false;
    join.disabled = false;

}

socket.onclose = () => {
    console.log("Disconnected from game server");
    connectstatus.textContent = "Connection Status: Not Connected"
    create.disabled = true;
    join.disabled = true;
}

function enableRemaining() {
    for (let i = 0; i < 3; i++) {
        for (let j = 0; j < 3; j++) {
            if (matrix[i][j].textContent === "") {
                matrix[i][j].disabled = false;
            }
        }
    }
}

function disableAll() {
    for (let i = 0; i < 3; i++) {
        for (let j = 0; j < 3; j++) {
            matrix[i][j].disabled = true;
        }
    }
}

function clearAndDisableAll() {
    for (let i = 0; i < 3; i++) {
        for (let j = 0; j < 3; j++) {
            matrix[i][j].textContent = "";
            matrix[i][j].disabled = true;
        }
    }
}

socket.onerror = (err) => {
    console.log("Socket error:", err);
    connectstatus.textContent = "Connection Status: Socket Error"
    create.disabled = true;
    join.disabled = true;
}

socket.onmessage = (event) => {
    const obj = JSON.parse(event.data);
    console.log(obj);
    if (obj["code"] === "MATCH_CREATED") {

        matchid.textContent = "Match ID : " + obj["matchid"];
        match_id = obj["matchid"];
        movestatus.textContent = "Waiting for Client...!";

    } else if (obj["code"] === "MATCH_JOINED") {

        matchid.textContent = "Match ID : " + obj["matchid"];
        match_id = obj["matchid"];

    } else if (obj["code"] === "MATCH_JOIN_FAILED") {

        alert("Failed to Join match");
        create.disabled = false;
        join.disabled = false;
        mid.disabled = false;
        mid.value = "";

    } else if (obj["code"] === "MOVE") {

        let i = Number(obj["message"][0]);
        let j = Number(obj["message"][1]);
        matrix[i][j].textContent = obj["player"];
        matrix[i][j].style.color = "#ff5252";
        matrix[i][j].disabled = true;
        enableRemaining();
        movestatus.textContent = "Your Move";

    } else if (obj["code"] === "INIT_BOARD") {
        if (player === 'X') {
            enableRemaining();
            movestatus.textContent = "Client Connected : Your Move";
        } else {
            movestatus.textContent = "Their Move";
        }
    } else if (obj["code"] === "GAME_OVER") {
        clearAndDisableAll();
        create.disabled = false;
        join.disabled = false;
        mid.disabled = false;
        mid.value = "";
        movestatus.textContent = obj["message"];
        matchid.textContent = "Match ID :";
    }
}

create.addEventListener("click", () => {
    create.disabled = true;
    join.disabled = true;
    mid.disabled = true;
    player = "X";
    const data = {
        "matchid": "0",
        "player": player,
        "code": "CREATE_MATCH",
        "message": " "
    }
    socket.send(JSON.stringify(data));
});

join.addEventListener("click", () => {

    const value = mid.value;          // string
    const num = mid.valueAsNumber;    // number

    if (value === "") {
        alert("Match ID required");
        return;
    }

    if (isNaN(num)) {
        alert("Invalid number");
        mid.value = "";
        return;
    }
    if (num < 0 || num >= 1000) {
        alert("Match ID must be between 0 and 999");
        mid.value = "";
        return;
    }

    create.disabled = true;
    join.disabled = true;
    mid.disabled = true;
    player = "O";

    const data = {
        matchid: "0",
        player: player,
        code: "JOIN_MATCH",
        message: num.toString()
    };

    socket.send(JSON.stringify(data));
});

function handleMove(i, j) {
    console.log(i + "," + j);
    matrix[i][j].textContent = player;
    matrix[i][j].style.color = "#003cff";
    matrix[i][j].disabled = true;

    
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: String(i)+String(j)
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
}

for (let i = 0; i < 3; i++) {
    for (let j = 0; j < 3; j++) {
        matrix[i][j].addEventListener("click", () => handleMove(i, j));
    }
}