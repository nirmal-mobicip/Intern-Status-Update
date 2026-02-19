let username;

while (!username) {
    username = prompt("ENTER USER NAME : ");
}


const statusH = document.getElementById("status");
const send = document.getElementById("send");
const message = document.getElementById("message");
const to = document.getElementById("to");

const socket = new WebSocket("ws://localhost:8080");

socket.onopen = () => {
    console.log("Connected to server");
    statusH.textContent = "Connected";
    statusH.style.color = "green";
    send.disabled = false;
    const data = {
            from: "client",
            to: "server",
            msg: username,
        };
        socket.send(JSON.stringify(data));
};

socket.onclose = () => {
    console.log("Disconnected from server");
    statusH.textContent = "Disconnected";
    statusH.style.color = "red";
    send.disabled = true;
};

socket.onerror = (error) => {
    console.log("Socket error:", error);
};

socket.onmessage = (event) => {
    console.log("Message from server:", event.data);
};


send.addEventListener("click", () => {
    if (message.value === "" || to.value === "") {
        console.log("Type Something and send");
    } else {
        const data = {
            from: "nk",
            to: to.value,
            msg: message.value,
        };
        socket.send(JSON.stringify(data));
        message.value = "";
    }
});

message.addEventListener("keypress", (e) => {
    if (e.key === "Enter") {
        if (message.value === "" || to.value === "") {
            console.log("Type Something and send");
        } else {

            const data = {
                from: "nk",
                to: to.value,
                msg: message.value,
            };
            socket.send(JSON.stringify(data));
            message.value = "";
        }
    }

});


