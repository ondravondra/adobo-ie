salsita.extensions = {};
salsita.extensions.sendRequest = function (request, callback) {
  _salsita_impl.sendRequest(request, callback);
};
salsita.extensions.onRequest = {}
salsita.extensions.onRequest.addListener = function (listener) {
  _salsita_impl.addRequestListener(listener);
};
salsita.tabs = {};
salsita.tabs.getCurrent = function (callback) {
  callback({ id: _salsita_impl.getCurrentTabId() });
};
salsita.tabs.sendRequest = function (tabId, request, callback) {
  _salsita_impl.sendRequestToTab(tabId, request, callback);
};
