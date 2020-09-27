namespace KHAxLib
{
    partial class KHAxControl
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(KHAxControl));
            this.api = new AxKHOpenAPILib.AxKHOpenAPI();
            ((System.ComponentModel.ISupportInitialize)(this.api)).BeginInit();
            this.SuspendLayout();
            // 
            // api
            // 
            this.api.Dock = System.Windows.Forms.DockStyle.Fill;
            this.api.Enabled = true;
            this.api.Location = new System.Drawing.Point(0, 0);
            this.api.Name = "api";
            this.api.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("api.OcxState")));
            this.api.Size = new System.Drawing.Size(800, 450);
            this.api.TabIndex = 0;
            // 
            // KHAxControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.api);
            this.Name = "KHAxControl";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.api)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public AxKHOpenAPILib.AxKHOpenAPI api;
    }
}

