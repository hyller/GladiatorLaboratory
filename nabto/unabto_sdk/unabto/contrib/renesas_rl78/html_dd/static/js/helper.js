/**
 *  Javascript helper functions for uNabto communication and functionality
 *  using JSON requests.
 */

/**
 *  Send query and receive response from uNabto device
 *  @param request   : string holding the query
 *  @param action    : function called on success
 */
function queryDevice(request, action) {

	// Send json request to device and implement callback function
	jQuery.getJSON(request, null, function(r) {
		$(".errors").text("");
		$(".errors").hide();

		var response = r["response"];
		console.dir(response);

		if (response != null) {
			action(response);
		}
	}).error(function (error){
		clearInterval(update_id);
		$(".errors").text("Communication error occurred!");
		$(".errors").show();
	});
}

/**
 *  Query functions
 *  Send and handle the different querys
 */

function queryAcc() {
	queryDevice("acceleration.json?", function(response) {
		if (response["acc_x"] != null) {
			var acc_x = ((response["acc_x"] - 256) / 33.0).toFixed(2);
			var acc_y = ((response["acc_y"] - 256) / 33.0).toFixed(2);
			var acc_z = ((response["acc_z"] - 256) / 33.0).toFixed(2);
			updateGraph(3, acc_x, acc_y, acc_z);
		}
	});
}

function queryTemp() {
	queryDevice("temperature.json?", function(response) {
		if (response["temp"] != null) {
			var temp = (response["temp"]/128.0).toFixed(2);
			updateGraph(1, temp, null, null);
		}
	});
}

function queryLight() {
	queryDevice("light_level.json?", function(response) {
		if (response["light"] != null) {
			var light = response["light"];
			updateGraph(1, light, null, null);
		}
	});
}

function queryPot() {
	queryDevice("potentiometer.json?", function(response) {
		if (response["pot"] != null) {
			var pot = response["pot"] / 40;
			updateGraph(1, pot, null, null);
		}
	});
}

function queryButton() {
	queryDevice("button_status.json?", function(response) {
		if (response["button_1"] != null) {
			$("#button1").attr("checked", response["button_1"]?true:false).checkboxradio("refresh");
			$("#button2").attr("checked", response["button_2"]?true:false).checkboxradio("refresh");
			$("#button3").attr("checked", response["button_3"]?true:false).checkboxradio("refresh");
		}
	});
}

function querySound() {
	queryDevice("sound_level.json?", function(response) {
		if (response["sound"] != null) {
			var sound = Math.abs((response["sound"]/4)-493);
			updateGraph(1, sound, null, null);
		}
	});
}

function queryLed(input) {
	var led = (input.val() == "off")?0:1;
	queryDevice("led.json?led_id=" + led, function(response){
    if (response["led_state"] != null) {
    	input.val(response["led_state"]?"on":"off").slider("refresh");
    }
	});
}

/**
 *  Extra helper functions
 *  Handle small tasks between button click and send of query
 */
 
var update_id;
var timeout_id;

function autoUpdate(input) {
	if (input.is(":checked")) {
		// Set update interval according to the caller
		var input_id = input.context.id;
		if (input_id == "acc_auto") {
			update_id = setInterval("queryAcc()", $("#acc_val").val());
		}
		else if (input_id == "temp_auto") {
			update_id = setInterval("queryTemp()", $("#temp_val").val());
		}
		else if (input_id == "light_auto") {
			update_id = setInterval("queryLight()", $("#light_val").val());
		}
		else if (input_id == "pot_auto") {
			update_id = setInterval("queryPot()", $("#pot_val").val());
		}
		else if (input_id == "sound_auto") {
			update_id = setInterval("querySound()", $("#sound_val").val());
		}
	} else {
		clearInterval(update_id);
		clearTimeout(timeout_id);
	}
}

var graph_counter = 0;
var graph_data1 = [[]];
var graph_data2 = [[]];
var graph_data3 = [[]];
var plot = 0;

function clearGraph() {
	clearInterval(update_id);
	clearTimeout(timeout_id);
	graph_data1 = [[]];
	graph_data2 = [[]];
	graph_data3 = [[]];
	plot = 0;

	// Fill arrays with initial data
	for (graph_counter = 0; graph_counter < 30; graph_counter++) {
		graph_data1[0].push([graph_counter, 0]);
		graph_data2[0].push([graph_counter, 0]);
		graph_data3[0].push([graph_counter, 0]);
	}
	graph_counter = 30;
}

function initGraph(input) {
	var input_id = input.context.id;
	
	options = {
    seriesDefaults: {
        rendererOptions: {
            smooth: true,
            animation: {
                show: true
            }
        },
				markerOptions: {
					lineWidth: 1.5,
					size: 5
				}
    },
		axesDefaults: {
			pad: 1.01
		},
		axes: {
			xaxis: {
				label: "time",
				labelRenderer: $.jqplot.CanvasAxisLabelRenderer
			},
			yaxis: {
				label: "",
				labelRenderer: $.jqplot.CanvasAxisLabelRenderer
			}
		},
		legend: {
			show: true,
			location: "sw"
		}
	};
	
	// Create graph chart depending on which page is loaded
	if (input_id == "acc-page") {
		options.axes.yaxis.label = "3d vector length";
		options.axes.yaxis.min = -1;
		options.axes.yaxis.max = 1;
		plot = $.jqplot("acc_chart", [graph_data1, graph_data2, graph_data3], options);
		plot.series[0].label = "X-axis";
		plot.series[1].label = "Y-axis";
		plot.series[2].label = "Z-axis";
		queryAcc();
	}
	else if (input_id == "temp-page") {
		options.axes.yaxis.label = "temp. \u00B0C";
		options.axes.yaxis.min = -100;
		options.axes.yaxis.max = 100;
		plot = $.jqplot("temp_chart", graph_data1, options);
		plot.series[0].label = "temperature";
		queryTemp();
	}
	else if (input_id == "light-page") {
		options.axes.yaxis.label = "lux";
		options.axes.yaxis.min = 0;
		options.axes.yaxis.max = 1000;
		plot = $.jqplot("light_chart", graph_data1, options);
		plot.series[0].label = "Light";
		queryLight();
	}
	else if (input_id == "pot-page") {
		options.axes.yaxis.label = "percent";
		options.axes.yaxis.min = 0;
		options.axes.yaxis.max = 100;
		plot = $.jqplot("pot_chart", graph_data1, options);
		plot.series[0].label = "Potentiometer";
		queryPot();
	}
	else if (input_id == "sound-page") {
		options.axes.yaxis.label = "sound level";
		options.axes.yaxis.min = 0;
		options.axes.yaxis.max = 30;
		plot = $.jqplot("sound_chart", graph_data1, options);
		plot.series[0].label = "Sound";
		querySound();
	}
}

function updateGraph(n,x,y,z) {
	// Update the correct amount of series
	if (n > 0) {
		var new_data1 = new Array();
		new_data1 = graph_data1;
		new_data1[0].shift();
		new_data1[0].push([graph_counter, x]);
		plot.series[0].data = new_data1[0];
	}
	if (n > 1) {
		var new_data2 = new Array();
		new_data2 = graph_data2;
		new_data2[0].shift();
		new_data2[0].push([graph_counter, y]);
		plot.series[1].data = new_data2[0];
	}
	if (n > 2) {
		var new_data3 = new Array();
		new_data3 = graph_data3;
		new_data3[0].shift();
		new_data3[0].push([graph_counter, z]);
		plot.series[2].data = new_data3[0];
	}
	
	graph_counter++;
	// Replot the graph without reseting the y-axis
	plot.replot({ resetAxes:["xaxis"] });
}
 
/**
 *  Startup functions
 *  Setup and bind html buttons to query send functions
 */

$("#frontpage").live("pageinit", function() {

	$("#acc_auto").click(function() {
		autoUpdate($(this));
	});
	$("#temp_auto").click(function() {
		autoUpdate($(this));
	});
	$("#light_auto").click(function() {
		autoUpdate($(this));
	});
	$("#pot_auto").click(function() {
		autoUpdate($(this));
	});
	$(".button_read").click(function() {
		queryButton();
	});
	$("#sound_auto").click(function() {
		autoUpdate($(this));
	});
	$("#led_toggle").change(function() {
		queryLed($(this));
	});
});

$("#frontpage").live("pageshow", function() {
	clearGraph();
});

$("#acc-page").live("pagebeforeshow", function() {
	initGraph($(this));
	timeout_id = setTimeout(function() {
		update_id = setInterval("queryAcc()", $("#acc_val").val());
	}, 2000);
});

$("#temp-page").live("pagebeforeshow", function() {
	initGraph($(this));
	timeout_id = setTimeout(function() {
		update_id = setInterval("queryTemp()", $("#temp_val").val());
	}, 2000);
});

$("#light-page").live("pagebeforeshow", function() {
	initGraph($(this));
	timeout_id = setTimeout(function() {
		update_id = setInterval("queryLight()", $("#light_val").val());
	}, 2000);
});

$("#pot-page").live("pagebeforeshow", function() {
	initGraph($(this));
	timeout_id = setTimeout(function() {
		update_id = setInterval("queryPot()", $("#pot_val").val());
	}, 2000);
});

$("#sound-page").live("pagebeforeshow", function() {
	initGraph($(this));
	timeout_id = setTimeout(function() {
		update_id = setInterval("querySound()", $("#sound_val").val());
	}, 2000);
});

