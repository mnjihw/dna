using Microsoft.ML;
using Microsoft.ML.Data;
using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.DataVisualization.Charting;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using WPFtoolkitFramework;
using System.IO;
using Microsoft.ML.Transforms.TimeSeries;
using System.Net.Http;

namespace ml.net_wpf
{
    public partial class MainWindow : Window
    {

        public class ModelInput
        {
            public DateTime Date { get; set; }
            public float Close { get; set; }
        }

        public class ModelOutput
        {
            public float[] ForecastedClose { get; set; }
            public float[] LowerBoundClose { get; set; }
            public float[] UpperBoundClose { get; set; }
        }


        void Evaluate(IDataView testData, ITransformer model, MLContext mlContext)
        {
            var predictions = model.Transform(testData);

            var actual = mlContext.Data.CreateEnumerable<ModelInput>(testData, true)
                    .Select(observed => observed.Close);

            var forecast = mlContext.Data.CreateEnumerable<ModelOutput>(predictions, true)
                    .Select(prediction => prediction.ForecastedClose[0]);

            var metrics = actual.Zip(forecast, (actualValue, forecastValue) => actualValue - forecastValue);

            var MAE = metrics.Average(error => Math.Abs(error)); // Mean Absolute Error
            var RMSE = Math.Sqrt(metrics.Average(error => Math.Pow(error, 2))); // Root Mean Squared Error

            // Output metrics
            Trace.WriteLine("Evaluation Metrics");
            Trace.WriteLine("---------------------");
            Trace.WriteLine($"Mean Absolute Error: {MAE:F3}");
            Trace.WriteLine($"Root Mean Squared Error: {RMSE:F3}\n");
        }

        void Forecast(IDataView testData, int horizon, TimeSeriesPredictionEngine<ModelInput, ModelOutput> forecaster, MLContext mlContext)
        {

            var forecast = forecaster.Predict();
            var actualPrices = new List<KeyValuePair<string, float>>();
            var forecastedPrices = new List<KeyValuePair<string, float>>();


            var forecastOutput = mlContext.Data.CreateEnumerable<ModelInput>(testData, reuseRowObject: false)
                    .Take(horizon)
                    .Select((ModelInput input, int index) =>
                    {
                        var date = input.Date;
                        float actualClose = input.Close;
                        float estimate = forecast.ForecastedClose[index];
                        float lowerEstimate = Math.Max(0, forecast.LowerBoundClose[index]);
                        float upperEstimate = forecast.UpperBoundClose[index];

                        actualPrices.Add(new KeyValuePair<string, float>(date.ToString(@"yy.MM.dd"), actualClose));
                        forecastedPrices.Add(new KeyValuePair<string, float>(date.ToString(@"yy.MM.dd"), estimate));

                        return $"Date: {date}\n" +
                        $"Actual Close: {actualClose}\n" +
                        $"Forecast: {estimate}\n" +
                        $"Lower Estimate: {lowerEstimate}\n" +
                        $"Upper Estimate: {upperEstimate}\n";
                    });


            (chart.Series[0] as LineSeries).ItemsSource = actualPrices;
            (chart.Series[1] as LineSeries).ItemsSource = forecastedPrices;


            Trace.WriteLine("Close Forecast");
            Trace.WriteLine("---------------------");
            foreach (var prediction in forecastOutput)
            {
                Trace.WriteLine(prediction);
            }
        }
        public MainWindow()
        {
            InitializeComponent();


            string rootDir = Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "../../../"));
            string modelPath = Path.Combine(rootDir, "MLModel.zip");
            var csv = Path.Combine(rootDir, @"Data\data.csv");

            var columns = new[]
            {
                new TextLoader.Column("Date", DataKind.DateTime, 0),
                new TextLoader.Column("Close", DataKind.Single, 4),
            };

            var mlContext = new MLContext();

            var count = File.ReadAllLines(csv).Length - 1;
          /*  var list = new List<string>();
            foreach (var line in File.ReadAllLines(csv))
            {
                if (!line.Contains("null"))
                {
                    list.Add(line);
                }
            }
            var count = list.Count - 1;*/

            var dataView = mlContext.Data.LoadFromTextFile(csv, columns, ',', hasHeader: true);
            //var dataView = mlContext.Data.LoadFromEnumerable(list);



            var trainSize = (int)(count * 0.5);
            var first = mlContext.Data.TakeRows(dataView, trainSize);
            var second = mlContext.Data.SkipRows(dataView, trainSize);
            
            
            var forecastCount = 256;

            var forecastingPipeline = mlContext.Forecasting.ForecastBySsa(
                outputColumnName: "ForecastedClose",
                inputColumnName: "Close",
                windowSize: 7,
                seriesLength: 30,
                trainSize: trainSize,
                horizon: count - trainSize,
                confidenceLevel: 0.95f,
                confidenceLowerBoundColumn: "LowerBoundClose",
                confidenceUpperBoundColumn: "UpperBoundClose");

            var forecaster = forecastingPipeline.Fit(first);

            Evaluate(second, forecaster, mlContext);

            var forecastEngine = forecaster.CreateTimeSeriesEngine<ModelInput, ModelOutput>(mlContext);
            forecastEngine.CheckPoint(mlContext, modelPath);

            Forecast(second, forecastCount, forecastEngine, mlContext);

            Trace.WriteLine("끝");



        }

    }
}
