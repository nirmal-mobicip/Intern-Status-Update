
const updateTip = async () => {
  const response = await fetch('https://chrome.dev/f/extension_tips/');
  const tips = await response.json();
  const randomIndex = Math.floor(Math.random() * tips.length);
  return chrome.storage.local.set({ tip: tips[randomIndex] });
};

const ALARM_NAME = 'tip';

async function createAlarm() {
  const alarm = await chrome.alarms.get(ALARM_NAME);
  if (typeof alarm === 'undefined') {
    chrome.alarms.create(ALARM_NAME, {
      delayInMinutes: 1,
      periodInMinutes: 1440
    });
    updateTip();
  }
}

createAlarm();

chrome.alarms.onAlarm.addListener(updateTip);

chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  if (message.greeting === 'tip') {
    chrome.storage.local.get('tip').then(sendResponse);
    return true;
  }
});