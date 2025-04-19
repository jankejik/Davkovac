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
       
      //  width = (data/2|0) +offset;
      width = data;
        if (width >= 97) {
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


function sendfile()
{
  var filedata = new FormData();
 /* if(last)
  {
    progress(50);
    filedata.append('file',$('#filefw')[0].files[0]);
  }
  else
  {*/
    progress(0);
    filedata.append('file',$('#filefs')[0].files[0]);
  //}
  $.ajax({
    url: "/update",
    data: filedata,
    cache: false,
    contentType: false,
    processData: false,
    type: "POST",
    success: function (data) {
      clearInterval(id);
      //if(last)
     // {
        $("#confModal").modal("show");
        setTimeout(function()
        {
            document.location.href="/";
        },15000);
        $("#myBar").css("width", "100%");
     /* }
      else
      {
        sendfile(true);
      }*/

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
  $("#upload").on("submit", function (c) {
    c.preventDefault();
    if(!$('#filefs').val()) {
      alert('Musíte vybrat soubor!');
      return;
   }
    $("#myProgress").css("display","block");
    $("#upload input[type=submit]").prop('disabled', true);
    sendfile();
  });
});
