var id;
function progress(offset) {
  console.log("Progress");
  var width = 0;
  id = setInterval(frame, 600);
  function frame() {
    $.ajax({
      type: "GET",
      url: "/progress",
      success: function (data) {
        width = (data/2|0) +offset;
        if (width >= 98) {
          width = "100";
        }
        var str = width + "%";
        if (width <= 10) {
          $("#myBar").css("width", "10%");
        } else {
          $("#myBar").css("width", str);
        }
        $("#myBar").html(str);
      },
    });
  }
}

function Actstatus() {
  console.log("Status");
  
  id = setInterval(statusData, 1000);
  function statusData() {
    $.ajax({
      type: "GET",
      url: "/statusData",
      success: function (data) {
        var ReadValue = data.split(";");
        $("#temp").val(ReadValue[0]/10 + " °C");
        $("#ph").val(ReadValue[1]/10 + " pH");
        $("#orp").val(ReadValue[2] + " mV");
        $("#flok").val(ReadValue[3] + " ml/m3 na den");
      },
    });
  }
}

function sendfile(last)
{
  var filedata = new FormData();
  if(last)
  {
    progress(50);
    filedata.append('file',$('#filefw')[0].files[0]);
  }
  else
  {
    progress(0);
    filedata.append('file',$('#filefs')[0].files[0]);
  }
  $.ajax({
    url: "/update",
    data: filedata,
    cache: false,
    contentType: false,
    processData: false,
    type: "POST",
    success: function (data) {
      console.log(data);
      clearInterval(id);
      if(last)
      {
        $("#confModal").modal("show");
        setTimeout(function()
        {
            document.location.href="/";
        },15000);
      }
      else
      {
        sendfile(true);
      }

    },
    error: function () {
      setTimeout(function()
      {
          document.location.href="/";
      },15000);
      alert("Při nahrávání došlo k chybě: zařízení se restartuje!");
    },
  });
}


$(document).ready(function () {
 
});
