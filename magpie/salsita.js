var apiImpl = _salsita_impl;

function createTab()
{
  return { id: apiImpl.currentTabId };
}

function createSender()
{
  var sender = {};
  sender.tab = createTab();
  if (sender.tab.id == -1)
  {
    delete sender.tab;
  }
  sender.id = apiImpl.extensionId;
  return sender;
}

salsita.extension = {};
salsita.extension.sendRequest = function (request, callback) {
  apiImpl.performSendRequest(-1, createSender(), request, callback);
};
salsita.extension.onRequest = {};
salsita.extension.onRequest.addListener = function (listener) {
  apiImpl.addEventListener('extension.onRequest', listener);
};
salsita.tabs = {};
salsita.tabs.getCurrent = function (callback) {
  var t = createTab();
  if (t.id == -1)
  {
    callback();
  } else {
    callback(t);
  }
};
salsita.tabs.sendRequest = function (tabId, request, callback) {
  apiImpl.performSendRequest(tabId, createSender(), request, callback);
};
salsita.tabs.onCreated = {};
salsita.tabs.onCreated.addListener = function (listener) {
  apiImpl.addEventListener('salsita.tabs.onCreated', listener);
};
salsita.tabs.onActivated = {};
salsita.tabs.onActivated.addListener = function (listener) {
  apiImpl.addEventListener('salsita.tabs.onActivated', listener);
};
salsita.tabs.onRemoved = {};
salsita.tabs.onRemoved.addListener = function (listener) {
  apiImpl.addEventListener('salsita.tabs.onRemoved', listener);
};
