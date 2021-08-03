using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.AspNetCore.ResponseCompression;
using Microsoft.AspNetCore.StaticFiles;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.FileProviders;
using Microsoft.Extensions.Hosting;
using System.IO;
using System.Linq;

namespace unity_test.Server
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        // For more information on how to configure your application, visit https://go.microsoft.com/fwlink/?LinkID=398940
        public void ConfigureServices(IServiceCollection services)
        {

            services.AddControllersWithViews();
            services.AddRazorPages();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
                app.UseWebAssemblyDebugging();
            }
            else
            {
                app.UseExceptionHandler("/Error");
                // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
                app.UseHsts();
            }



            app.UseHttpsRedirection();
            app.UseBlazorFrameworkFiles();
            var mimeTypeProvider = new FileExtensionContentTypeProvider();


            /*
             * 게임 실행시 필요한 파일
             * 1. WebGL.loader.js
             * 2. WebGL.framework.js.gz
             * 3. WebGL.data.gz
             * 4. WebGL.wasm.gz
             * 
             * 1번 파일을 제외한 나머지 파일은 아래 코드를 실행해야 정상작동함.
             */

            app.UseStaticFiles(new StaticFileOptions
            {
                OnPrepareResponse = context =>
                {
                    var headers = context.Context.Response.Headers;
                    var contentType = headers["Content-Type"];

                    if (contentType != "application/x-gzip" && !context.File.Name.EndsWith(".gz"))
                        return;

                    var fileNameToTry = Path.GetFileNameWithoutExtension(context.File.Name);

                    headers.Add("Content-Encoding", "gzip");

                    if (mimeTypeProvider.TryGetContentType(fileNameToTry, out var mimeType))
                        headers["Content-Type"] = mimeType;   
                }
            });

            app.UseStaticFiles();

            app.UseRouting();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapRazorPages();
                endpoints.MapControllers();
                //endpoints.MapFallbackToFile("index.html");
                endpoints.MapFallbackToFile("index2.html");
            });
        }
    } 
}
