
const create = document.getElementById("create");
const join = document.getElementById("join");
const connectstatus = document.getElementById("status");

let player;



const socket = new WebSocket("ws://127.0.0.1:8080")

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

socket.onerror = (err) => {
    console.log("Socket error:", err);
    connectstatus.textContent = "Connection Status: Socket Error"
    create.disabled = true;
    join.disabled = true;
}

socket.onmessage = (event) => {
    
}

create.addEventListener("click",()=>{
    create.disabled = true;
    join.disabled = true;
    player = "X";
    const data = {
        "matchid" : "0",
        "player" : player,
        "code" : "CREATE_MATCH",
        "message" : " "
    }
    socket.send(JSON.stringify(data));
});