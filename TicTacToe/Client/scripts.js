
const create = document.getElementById("create");
const join = document.getElementById("join");
const connectstatus = document.getElementById("status");
const matchid = document.getElementById("matchid");
const mid = document.getElementById("mid");
const movestatus = document.getElementById("movestatus");

const a0b0 = document.getElementById("a0b0");
const a0b1 = document.getElementById("a0b1");
const a0b2 = document.getElementById("a0b2");
const a1b0 = document.getElementById("a1b0");
const a1b1 = document.getElementById("a1b1");
const a1b2 = document.getElementById("a1b2");
const a2b0 = document.getElementById("a2b0");
const a2b1 = document.getElementById("a2b1");
const a2b2 = document.getElementById("a2b2");

let player, match_id;



const socket = new WebSocket("ws://192.168.31.65:8080")

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

function enableAll() {
    a0b0.disabled = false;
    a0b1.disabled = false;
    a0b2.disabled = false;
    a1b0.disabled = false;
    a1b1.disabled = false;
    a1b2.disabled = false;
    a2b0.disabled = false;
    a2b1.disabled = false;
    a2b2.disabled = false;
}

function disableAll() {
    a0b0.disabled = true;
    a0b1.disabled = true;
    a0b2.disabled = true;
    a1b0.disabled = true;
    a1b1.disabled = true;
    a1b2.disabled = true;
    a2b0.disabled = true;
    a2b1.disabled = true;
    a2b2.disabled = true;
}

function clearAll(){
    a0b0.textContent = " ";
    a0b1.textContent = " ";
    a0b2.textContent = " ";
    a1b0.textContent = " ";
    a1b1.textContent = " ";
    a1b2.textContent = " ";
    a2b0.textContent = " ";
    a2b1.textContent = " ";
    a2b2.textContent = " ";
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
    } else if (obj["code"] === "MOVE") {
        if (obj["message"] === "00") {
            a0b0.textContent = obj["player"];
            a0b0.style.color = "#ff5252";
            a0b0.disabled = true;
        } else if (obj["message"] === "01") {
            a0b1.textContent = obj["player"];
            a0b1.style.color = "#ff5252";
            a0b1.disabled = true;
        } else if (obj["message"] === "02") {
            a0b2.textContent = obj["player"];
            a0b2.style.color = "#ff5252";
            a0b2.disabled = true;
        } else if (obj["message"] === "10") {
            a1b0.textContent = obj["player"];
            a1b0.style.color = "#ff5252";
            a1b0.disabled = true;
        } else if (obj["message"] === "11") {
            a1b1.textContent = obj["player"];
            a1b1.style.color = "#ff5252";
            a1b1.disabled = true;
        } else if (obj["message"] === "12") {
            a1b2.textContent = obj["player"];
            a1b2.style.color = "#ff5252";
            a1b2.disabled = true;
        } else if (obj["message"] === "20") {
            a2b0.textContent = obj["player"];
            a2b0.style.color = "#ff5252";
            a2b0.disabled = true;
        } else if (obj["message"] === "21") {
            a2b1.textContent = obj["player"];
            a2b1.style.color = "#ff5252";
            a2b1.disabled = true;
        } else if (obj["message"] === "22") {
            a2b2.textContent = obj["player"];
            a2b2.style.color = "#ff5252";
            a2b2.disabled = true;
        }
        enableAll();
        movestatus.textContent = "Your Move";
    } else if (obj["code"] === "INIT_BOARD") {
        if (player === 'X') {
            enableAll();
            movestatus.textContent = "Client Connected : Your Move";
        } else {
            movestatus.textContent = "Their Move";
        }
    } else if (obj["code"] === "GAME_OVER") {
        alert(obj["message"]);
        clearAll();
        disableAll();
        create.disabled = false;
        join.disabled = false;
        mid.disabled = false;
        mid.value = "";
        movestatus.textContent = "";
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
        return;
    }
    if (num < 0 || num >= 1000) {
        alert("Match ID must be between 0 and 999");
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

a0b0.addEventListener("click", () => {
    console.log("0,0");
    a0b0.textContent = player;
    a0b0.style.color = "#003cff";
    a0b0.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "00"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
});

a0b1.addEventListener("click", () => {
    console.log("0,1");
    a0b1.textContent = player;
    a0b1.style.color = "#003cff";
    a0b1.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "01"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
a0b2.addEventListener("click", () => {
    console.log("0,2");
    a0b2.textContent = player;
    a0b2.style.color = "#003cff";
    a0b2.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "02"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
a1b0.addEventListener("click", () => {
    console.log("1,0");
    a1b0.textContent = player;
    a1b0.style.color = "#003cff";
    a1b0.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "10"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
a1b1.addEventListener("click", () => {
    console.log("1,1");
    a1b1.textContent = player;
    a1b1.style.color = "#003cff";
    a1b1.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "11"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
a1b2.addEventListener("click", () => {
    console.log("1,2");
    a1b2.textContent = player;
    a1b2.style.color = "#003cff";
    a1b2.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "12"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
a2b0.addEventListener("click", () => {
    console.log("2,0");
    a2b0.textContent = player;
    a2b0.style.color = "#003cff";
    a2b0.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "20"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})

a2b1.addEventListener("click", () => {
    console.log("2,1");
    a2b1.textContent = player;
    a2b1.style.color = "#003cff";
    a2b1.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "21"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
a2b2.addEventListener("click", () => {
    console.log("2,2");
    a2b2.textContent = player;
    a2b2.style.color = "#003cff";
    a2b2.disabled = true;
    const data = {
        matchid: match_id,
        player: player,
        code: "MOVE",
        message: "22"
    }
    socket.send(JSON.stringify(data));
    disableAll();
    movestatus.textContent = "Their Move";
})
