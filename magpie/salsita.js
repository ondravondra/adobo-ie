var apiImpl = _salsita_impl;

function getParam(obj, name, defVal)
{
  if (name in obj) {
    return obj[name];
  } else {
    return defVal;
  }
}

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
salsita.extension.getURL = function (url) {
  return apiImpl.getResourceUrl(url);
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
  apiImpl.addEventListener('tabs.onCreated', listener);
};
salsita.tabs.onActivated = {};
salsita.tabs.onActivated.addListener = function (listener) {
  apiImpl.addEventListener('tabs.onActivated', function (_tabId) { listener({ tabId: _tabId }); });
};
salsita.tabs.onRemoved = {};
salsita.tabs.onRemoved.addListener = function (listener) {
  apiImpl.addEventListener('tabs.onRemoved', listener);
};

// content script api

if (typeof _salsita_content_impl !== "undefined") {
  var contentApiImpl = _salsita_content_impl;

  salsita.content.openNewTab = function (p) {
    return contentApiImpl.openNewTab(p.url, getParam(p, 'active', true));
  };

  salsita.content.navigateTo = function (p) {
    return contentApiImpl.navigateTo(p.url);
  };

  salsita.content.openPopupWindow = function (p, callback) {
    return contentApiImpl.openPopupWindow(
      p.url,
      getParam(p, 'left', -1),
      getParam(p, 'top', -1),
      getParam(p, 'width', -1),
      getParam(p, 'height', -1),
      callback);
  };
}
