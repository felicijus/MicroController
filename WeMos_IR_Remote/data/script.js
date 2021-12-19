

var x = document.querySelectorAll(" div.buttonrow > button");
var i;
for (i = 0; i < x.length; i++) {

  x[i].addEventListener('click', event => {
    console.log( event.target.id );

    var xhttp = new XMLHttpRequest();
    // http://example.com/servlet?parameter=value 
    xhttp.open("GET", "/set?"+event.target.innerHTML+"="+event.target.id, true);
    xhttp.send();
});

}

