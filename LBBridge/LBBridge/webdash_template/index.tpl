<!DOCTYPE html>
<html>

<head>
  <meta http-equiv="refresh" content="15">
  <style>
    * {
      font-family: Arial, Helvetica, sans-serif;
    }

    p {
      margin: 0;
    }

    table,
    th,
    td {
      border: 1px solid black;
      border-collapse:separate;
    }

    .none {
      font-weight: bold;
      color: black;
    }

    .ok {
      font-weight: bold;
      color: green;
    }

    .warn {
      font-weight: bold;
      color: yellow;
    }

    .error {
      font-weight: bold;
      color: red;
    }
    
    td {
      padding-left: 0.3em;
      padding-right: 0.3em;
    }
  </style>
</head>

<body>

  <img src='/static/logo.svg' width="480" />

  <h1>{{title}}</h1>

  <table>
    % for item in status:
    <tr>
      <td>{{!item[0]}}</td>
      <td class='status'>{{!item[1]}}</td>
    </tr>
    % end
  </table>

  <br />
  <br />
  
  <h3>Service Logs</h3>
  <p><a href="/service_log/lbnetwork">Network Log</a></p>
  <p><a href="/service_log/lbrelay">Relay Log</a></p>
</body>

</html>