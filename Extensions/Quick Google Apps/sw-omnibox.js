chrome.runtime.onInstalled.addListener(({ reason }) => {
  if (reason === 'install') {
    chrome.storage.local.set({
      apiSuggestions: ['mail', 'docs', 'maps', 'drive']
    });
  }
});


const URL_BEGIN = 'https://';
const URL_END = '.google.com/';

const NUMBER_OF_PREVIOUS_SEARCHES = 4;

chrome.omnibox.onInputChanged.addListener(async (input, suggest) => {
  await chrome.omnibox.setDefaultSuggestion({
    description: 'Enter a Google App to open'
  });
  const { apiSuggestions } = await chrome.storage.local.get('apiSuggestions');
  const suggestions = apiSuggestions.map((api) => {
    return { content: api, description: `Open https://${api}.google.com` };
  });
  suggest(suggestions);
});

chrome.omnibox.onInputEntered.addListener((input) => {
  chrome.tabs.create({
    url: URL_BEGIN + input + URL_END
  });
  updateHistory(input);
});

async function updateHistory(input) {
  const { apiSuggestions } = await chrome.storage.local.get('apiSuggestions');
  apiSuggestions.unshift(input);
  apiSuggestions.splice(NUMBER_OF_PREVIOUS_SEARCHES);
  return chrome.storage.local.set({ apiSuggestions });
}