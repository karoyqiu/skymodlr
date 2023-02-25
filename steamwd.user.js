// ==UserScript==
// @name           SteamWorkshop.Download Button
// @namespace      http://vova1234.com/blog/web/69.html
// @description    Adds a button to the Steam Workshop pages that lets you head straight to the specific addon page at steamworkshop.download
// @include        *steamcommunity.com/sharedfiles/filedetails/?id=*
// @version        0.0.1
// @downloadURL    http://st.abcvg.info/swd/steamwd.user.js
// ==/UserScript==

const id = /[0-9]{2,15}/.exec(document.URL);

// WebChannel
new QWebChannel(qt.webChannelTransport, (channel) => {
    console.log('Web channel connected!', id);
});

const realButton = document.getElementById('SubscribeItemBtn');

const button = document.createElement('a');
button.setAttribute('id', realButton.id);
button.setAttribute('class', realButton.className);
button.setAttribute('href', `dl:${id}`);
button.innerHTML = '<div class="subscribeIcon"></div>' +
    '<span class="subscribeText">' +
    '<div id="SubscribeItemOptionAdd" class="subscribeOption subscribe selected">{{download}}</div>' +
    '</span>';

// append the element after the real subscribe button
if (realButton.nextSibling) {
    realButton.parentNode.insertBefore(button, realButton.nextSibling);
} else {
    realButton.parentNode.appendChild(button);
}
