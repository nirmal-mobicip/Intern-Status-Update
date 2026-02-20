let username;

while (!username) {
    username = prompt("ENTER USER NAME : ");
    if (username === "broadcast") {
        username = null;
    }
}

const uname = document.getElementById("uname");
uname.textContent = uname.textContent + username;

const statusH = document.getElementById("status");
const send = document.getElementById("send");
const message = document.getElementById("message");
const to = document.getElementById("to");
const messages = document.getElementById("messages");
const clients = document.getElementById("clients");

const chat = {}
const client_names = new Set();

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
    obj = JSON.parse(event.data);
    console.log(obj);
    const entry = obj["from"] + " : " + obj["msg"] + "\n";
    if (chat[obj["from"]] == null) {
        chat[obj["from"]] = entry;
    } else {
        chat[obj["from"]] += entry;
    }
    messages.value += entry;
    if(!(obj["from"].includes("BroadCast from")) && !client_names.has(obj["from"])){
        // add to data item
        if(!client_names.has(obj["to"])){
            client_names.add(obj["to"]);
            // add to dataitem
            const child = document.createElement("option");
            child.value = obj["to"];
            clients.appendChild(child);            
        }
    }
};


send.addEventListener("click", () => {
    if (message.value === "" || to.value === "") {
        console.log("Type Something and send");
    } else {
        const data = {
            "from": username,
            "to": to.value,
            "msg": message.value,
        };
        console.log("Sending obj");
        console.log(data);

        const entry = "You : " + data["msg"] + "\n";
        if (chat[data["from"]] == null) {
            chat[data["from"]] = entry;
        } else {
            chat[data["from"]] += entry;
        }
        messages.value += entry;
        socket.send(JSON.stringify(data));
        message.value = "";

        if(!client_names.has(data["to"])){
            client_names.add(data["to"]);
            // add to dataitem
            const child = document.createElement("option");
            child.value = data["to"];
            clients.appendChild(child);            
        }
    }
});

message.addEventListener("keypress", (e) => {
    if (e.key === "Enter") {
        if (message.value === "" || to.value === "") {
            console.log("Type Something and send");
        } else {

            const data = {
                "from": username,
                "to": to.value,
                "msg": message.value,
            };
            console.log("Sending obj");
            console.log(data);
            messages.value += "You : " + data["msg"] + "\n";
            socket.send(JSON.stringify(data));
            message.value = "";

            if(!client_names.has(data["to"])){
            client_names.add(data["to"]);
            // add to dataitem
            const child = document.createElement("option");
            child.value = data["to"];
            clients.appendChild(child);            
        }
        }
    }

});

to.addEventListener("input", e => {
  console.log(e.target.value);

    // add the content of user in textarea

});

to.addEventListener("focus", function () {
  to.value = ""
});
