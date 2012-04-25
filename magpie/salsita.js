function createTab()
{
  var tab = { id: _salsita_impl.currentTabId };
  return tab;
}

function createSender()
{
  var sender = {};
  sender.tab = createTab();
  if (sender.tab.id == -1)
  {
    delete sender.tab;
  }
  sender.id = _salsita_impl.extensionId;
  return sender;
}

salsita.extension = {};
salsita.extension.sendRequest = function (request, callback) {
  _salsita_impl.performSendRequest(-1, createSender(), request, callback);
};
salsita.extension.onRequest = {}
salsita.extension.onRequest.addListener = function (listener) {
  _salsita_impl.addRequestListener(listener);
};
salsita.tabs = {};
salsita.tabs.getCurrent = function (callback) {
  callback(createTab());
};
salsita.tabs.sendRequest = function (tabId, request, callback) {
  _salsita_impl.performSendRequest(tabId, createSender(), request, callback);
};
