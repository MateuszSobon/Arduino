const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" rel="stylesheet">
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h1 { font-size: 2.0rem; }
      p { font-size: 2.0rem; }
      .units { font-size: 1.2rem; }
      .dht-labels{
        font-size: 1.5rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
    </style>
  </head>
  <body>
    <h1>Stacja podlewania</h1>
    <p>
      <span id="data">0</span>
    </p>

    <script>
      setInterval(function() // Call a function repetatively with 2 Second interval
      {                 
        getTemperatureData();
      } , 2000); 
      

      function getTemperatureData() 
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() 
        {
          if (this.readyState == 4 && this.status == 200) 
          {
            document.getElementById("data").innerHTML =
            this.responseText;
          }
        };
        xhttp.open("GET", "readData", true);
        xhttp.send();
      }

      
    </script>
  </body>
</html>
)=====";
