const statusH = document.getElementById("status");
const send = document.getElementById("send");
const message = document.getElementById("message");

const socket = new WebSocket("ws://localhost:8080");

socket.onopen = () => {
    console.log("Connected to server");
    statusH.textContent = "Connected";
    statusH.style.color = "green";
    send.disabled = false;
};

socket.onclose = () => {
    console.log("Disconnected");
};

socket.onerror = (error) => {
    console.log("Socket error:", error);
};

socket.onmessage = (event) => {
    console.log("Message from server:", event.data);
};


send.addEventListener("click",() => {
    socket.send(message.value);
    message.value = "";
});

message.addEventListener("keypress",(e)=>{
    if(e.key === "Enter"){
        socket.send(message.value);
    message.value = "";
    }
});


