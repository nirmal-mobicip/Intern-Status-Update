let username;

if (!username) {
    while (!username) {
        username = prompt("ENTER USER NAME:");
        if (!username || username.toLowerCase() === "broadcast") {
            username = null;
        }
    }
}

const unameH = document.getElementById("uname");
unameH.textContent += username;



const statusH = document.getElementById("status");
const sendBtn = document.getElementById("send");
const messageBox = document.getElementById("message");
const toInput = document.getElementById("to");
const messagesArea = document.getElementById("messages");
const clientsList = document.getElementById("clients");


const chat = {};                // conversation per user
const clientNames = new Set();  // known clients


const socket = new WebSocket("ws://localhost:8080");

socket.onopen = () => {
    console.log("Connected");
    statusH.textContent = "Connected";
    statusH.style.color = "green";
    sendBtn.disabled = false;

    // send username to server
    socket.send(JSON.stringify({
        from: "client",
        to: "server",
        msg: username
    }));
};

socket.onclose = () => {
    console.log("Disconnected");
    statusH.textContent = "Disconnected";
    statusH.style.color = "red";
    sendBtn.disabled = true;
};

socket.onerror = (err) => {
    console.log("Socket error:", err);
};

socket.onmessage = (event) => {
    const obj = JSON.parse(event.data);

    const entry = obj.from + " : " + obj.msg + "\n";

    addToChat(obj.from, entry);
    messagesArea.value += entry;

    addClient(obj.from);
};


function addToChat(user, entry) {
    if (!chat[user]) {
        chat[user] = entry;
    } else {
        chat[user] += entry;
    }
}

function addClient(name) {
    if (!name || clientNames.has(name) || name === username) return;

    clientNames.add(name);

    const option = document.createElement("option");
    option.value = name;
    clientsList.appendChild(option);
}

function sendMessage() {
    const msg = messageBox.value.trim();
    const toUser = toInput.value.trim();

    if (!msg || !toUser) {
        console.log("Type message and recipient");
        return;
    }

    const data = {
        from: username,
        to: toUser,
        msg: msg
    };

    const entry = "You : " + msg + "\n";

    addToChat(toUser, entry);
    messagesArea.value += entry;

    socket.send(JSON.stringify(data));

    messageBox.value = "";

    addClient(toUser);
}

sendBtn.addEventListener("click", sendMessage);

messageBox.addEventListener("keypress", (e) => {
    if (e.key === "Enter") {
        e.preventDefault();
        sendMessage();
    }
});

toInput.addEventListener("input", (e) => {
    const user = e.target.value;

    if (!chat[user]) {
        messagesArea.value = "";
    } else {
        messagesArea.value = chat[user];
    }
});

toInput.addEventListener("focus", () => {
    toInput.value = "";
});

