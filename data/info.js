var myChart;
var isChartUsed = false;

function Actstatus() {
  id = setInterval(statusData, 1000);
  function statusData() {
    $.ajax({
      type: "GET",
      url: "/statusData",
      success: function (data) {
        var ReadValue = data.split(";");
        $("#temp").html(ReadValue[0] / 10 + " °C");
        $("#ph").html(ReadValue[1] / 10 + " pH");
        $("#orp").html(ReadValue[2] + " mV");
        $("#flok").html(ReadValue[3] + " ml/m3");
      },
    });
  }
}

var ypH = [];
var yORP = [];
var yTemp = [];

function LoadValues(hours) {
  var par="TMP="+hours;
  $.ajax({
    type: "GET",
    url: "/chartpH",
    data: par,
    success: function (data) {
      ypH = data.split(";");
      for (var i = 0, len = ypH.length; i < len; i++) {
        ypH[i] = (parseInt(ypH[i], 10) / 7.5 + 60) / 10;
      }
      ypH.reverse();
      LoadChartORP(hours);
    },
  });
}

function LoadChartORP(hours) {
  var par="TMP="+hours;
  $.ajax({
    type: "GET",
    url: "/chartORP",
    data: par,
    success: function (data) {
      yORP = data.split(";");
      for (var i = 0, len = yORP.length; i < len; i++) {
        yORP[i] = parseInt(yORP[i], 10) * 6;
      }
      yORP.reverse();
      LoadChartTemp(hours);
    },
  });
}

function LoadChartTemp(hours) {
  var par="TMP="+hours;
  $.ajax({
    type: "GET",
    url: "/chartTemp",
    data: par,
    success: function (data) {
      yTemp = data.split(";");
      for (var i = 0, len = yTemp.length; i < len; i++) {
        yTemp[i] = parseInt(yTemp[i], 10) * 3 / 10;
      }
      yTemp.reverse();
      CreateChart(hours);
    },
  });
}

function CreateChart(hours) {
  var ctx = document.getElementById("myChart").getContext("2d");
  var actTime=new Date();
  var max=actTime.getTime();
  let dif = parseInt(hours)*3600000;
  var min = actTime.getTime()- dif;
  var xlabels = []
  var step = dif / 240;
  for (let i = min; i < max; i += step) {
    var tmp = new Date();
    tmp.setTime(i);
    xlabels.push(tmp);
  }
var unitstep;
  if(hours=="1")
  unitstep=10;
  else if(hours=="4")
  unitstep=30;
  else if(hours=="24")
  unitstep=180;

  var startTime = new Date();
  startTime.setTime(min);
 // var xlabels = GenLabels();
if(isChartUsed)
{
  myChart.destroy();
}
isChartUsed=true;
  myChart = new Chart(ctx, {
    type: "line",
    data: {
      labels: xlabels,
      datasets: [
        {
          label: "pH",
          data: ypH,
          yAxisID: 'ph',
          backgroundColor: "transparent",
          borderColor: "rgba(40,167,69,1)",
          lineTension: 0,
          pointRadius: 0,
        //  borderWidth: 5,
          pointHitRadius: 4
        },
        {
          label: "ORP",
          data: yORP,
          yAxisID: 'orp',

          backgroundColor: "transparent",
          borderColor: "rgba(255,140,0,1)",
          lineTension: 0,
          pointRadius: 0,
          pointHitRadius: 4
        },
        {
          label: "Teplota",
          data: yTemp,
          yAxisID: 'tep',
          backgroundColor: "transparent",
          borderColor: "rgba(0,0,255,1)",
          lineTension: 0,
          pointRadius: 0,
          pointHitRadius: 4
        },
      ],
    },
    options: {
      legend: {
        align: "center"
      },
      scales: {
        xAxes: [{
          
          type: 'time',
          time: {
            min: startTime,
          max: actTime,
            unit: 'minute',
            unitStepSize: unitstep,
            displayFormats: { 'minute': 'HH:mm' },
            tooltipFormat: 'HH:mm'
          }, 
          scaleLabel: {
            display: true,
        //    fontSize: 16,
            labelString: 'Čas [h]'
          }
        }],
        yAxes: [{
          afterFit: function(scale) {
            scale.width = 45
         },
          id: 'ph',
          type: 'linear',
          position: 'left',
          ticks: {
            fontColor: "rgba(40,167,69,1)",
       //     fontSize: 12,
            max: 8,
            min: 6
          },
          scaleLabel: {
           
            display: true,
    //        fontSize: 12,
            labelString: 'pH'
          }
        }, {
          afterFit: function(scale) {
            scale.width = 45
         },
          id: 'orp',
          type: 'linear',
          position: 'left',

          ticks: {
            fontColor: "rgba(255,140,0,1)",
        //    fontSize: 12,
            max: 1000,
            min: 0
          },
          scaleLabel: {
            display: true,
        //    fontSize: 12,
            labelString: 'ORP [mV]'
          }
        },
        {
          afterFit: function(scale) {
            scale.width = 45
         },
          id: 'tep',
          type: 'linear',
          position: 'right',
          ticks: {
            fontColor: "rgba(0,0,255,1)",
         //   fontSize: 12,
            max: 50,
            min: 0
          },
          scaleLabel: {
            display: true,
        //    fontSize: 12,
            labelString: 'Teplota [°C]'
          }
        }]
      }
    }
  });
}

function GenLabels() {
  var actTime=new Date();
  var max=actTime.getTime();
  var min = actTime.getTime()-14400000;
  var labels = []
  var step = 14400000 / 240;
  for (let i = min; i < max; i += step) {
    var tmp = new Date();
    tmp.setTime(i);
    labels.push(tmp);
  }
  return labels
}


function PostAutoState(idauto, ischecked) {
  autdata = idauto.concat("=", (ischecked ? 1 : 0));
  $.ajax({
    type: "POST",
    url: "/automatic",
    data: autdata,
    success: function () {
    },
  });
}

function GetAutoState() {

  id = setInterval(autoData, 3000);
  function autoData() {
    $.ajax({
      type: "GET",
      url: "/getautostate",
      success: function (data) {
        var ReadValue = data.split(";");
        $('#phplus').prop('checked', parseInt(ReadValue[0]));
        $('#phminus').prop('checked', parseInt(ReadValue[1]));
        $('#chlor').prop('checked', parseInt(ReadValue[2]));
        $('#floku').prop('checked', parseInt(ReadValue[3]));
      },
    });
  }
}

const toggle = document.querySelector('.toggle input')


$(document).ready(function () {
  isChartUsed = false;
  Actstatus();
 LoadValues($('#chartselect').val());
 $('#chartselect').on('change', function() {
  LoadValues($('#chartselect').val());
});
  GetAutoState();
  $(':checkbox').change(function () {
    PostAutoState($(this).attr("id"), $(this).is(":checked"));
  });
});
