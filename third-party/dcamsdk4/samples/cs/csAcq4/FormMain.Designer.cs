namespace csAcq4
{
    partial class FormMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.LabelStatus = new System.Windows.Forms.Label();
            this.LabelLutMin = new System.Windows.Forms.Label();
            this.LabelLutMax = new System.Windows.Forms.Label();
            this.EditLutMin = new System.Windows.Forms.TextBox();
            this.EditLutMax = new System.Windows.Forms.TextBox();
            this.PushAsterisk = new System.Windows.Forms.Button();
            this.HScrollLutMin = new System.Windows.Forms.HScrollBar();
            this.HScrollLutMax = new System.Windows.Forms.HScrollBar();
            this.PicDisplay = new System.Windows.Forms.PictureBox();
            this.PushInit = new System.Windows.Forms.Button();
            this.PushOpen = new System.Windows.Forms.Button();
            this.PushInfo = new System.Windows.Forms.Button();
            this.PushProperties = new System.Windows.Forms.Button();
            this.PushSnap = new System.Windows.Forms.Button();
            this.PushLive = new System.Windows.Forms.Button();
            this.PushIdle = new System.Windows.Forms.Button();
            this.PushFireTrigger = new System.Windows.Forms.Button();
            this.PushBufRelease = new System.Windows.Forms.Button();
            this.PushClose = new System.Windows.Forms.Button();
            this.PushUninit = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.PicDisplay)).BeginInit();
            this.SuspendLayout();
            // 
            // LabelStatus
            // 
            this.LabelStatus.BackColor = System.Drawing.SystemColors.Control;
            this.LabelStatus.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelStatus.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelStatus.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelStatus.Location = new System.Drawing.Point(14, 14);
            this.LabelStatus.Name = "LabelStatus";
            this.LabelStatus.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelStatus.Size = new System.Drawing.Size(517, 17);
            this.LabelStatus.TabIndex = 0;
            // 
            // LabelLutMax
            // 
            this.LabelLutMax.BackColor = System.Drawing.SystemColors.Control;
            this.LabelLutMax.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelLutMax.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelLutMax.Location = new System.Drawing.Point(14, 38);
            this.LabelLutMax.Name = "LabelLutMax";
            this.LabelLutMax.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelLutMax.Size = new System.Drawing.Size(52, 17);
            this.LabelLutMax.TabIndex = 1;
            this.LabelLutMax.Text = "LUT Max";
            // 
            // EditLutMax
            // 
            this.EditLutMax.AcceptsReturn = true;
            this.EditLutMax.BackColor = System.Drawing.SystemColors.Window;
            this.EditLutMax.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.EditLutMax.ForeColor = System.Drawing.SystemColors.WindowText;
            this.EditLutMax.Location = new System.Drawing.Point(67, 38);
            this.EditLutMax.MaxLength = 0;
            this.EditLutMax.Name = "EditLutMax";
            this.EditLutMax.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.EditLutMax.Size = new System.Drawing.Size(50, 20);
            this.EditLutMax.TabIndex = 2;
            this.EditLutMax.TextChanged += new System.EventHandler(this.EditLutMax_TextChanged);
            // 
            // HScrollLutMax
            // 
            this.HScrollLutMax.Cursor = System.Windows.Forms.Cursors.Default;
            this.HScrollLutMax.LargeChange = 1;
            this.HScrollLutMax.Location = new System.Drawing.Point(122, 38);
            this.HScrollLutMax.Maximum = 32767;
            this.HScrollLutMax.Name = "HScrollLutMax";
            this.HScrollLutMax.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.HScrollLutMax.Size = new System.Drawing.Size(354, 17);
            this.HScrollLutMax.TabIndex = 3;
            this.HScrollLutMax.TabStop = true;
            this.HScrollLutMax.ValueChanged += new System.EventHandler(this.HScrollLutMax_ValueChanged);
            // 
            // LabelLutMin
            // 
            this.LabelLutMin.BackColor = System.Drawing.SystemColors.Control;
            this.LabelLutMin.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelLutMin.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelLutMin.Location = new System.Drawing.Point(14, 60);
            this.LabelLutMin.Name = "LabelLutMin";
            this.LabelLutMin.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelLutMin.Size = new System.Drawing.Size(52, 17);
            this.LabelLutMin.TabIndex = 4;
            this.LabelLutMin.Text = "LUT Min";
            // 
            // EditLutMin
            // 
            this.EditLutMin.AcceptsReturn = true;
            this.EditLutMin.BackColor = System.Drawing.SystemColors.Window;
            this.EditLutMin.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.EditLutMin.ForeColor = System.Drawing.SystemColors.WindowText;
            this.EditLutMin.Location = new System.Drawing.Point(67, 60);
            this.EditLutMin.MaxLength = 0;
            this.EditLutMin.Name = "EditLutMin";
            this.EditLutMin.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.EditLutMin.Size = new System.Drawing.Size(50, 20);
            this.EditLutMin.TabIndex = 5;
            this.EditLutMin.TextChanged += new System.EventHandler(this.EditLutMin_TextChanged);
            // 
            // HScrollLutMin
            // 
            this.HScrollLutMin.Cursor = System.Windows.Forms.Cursors.Default;
            this.HScrollLutMin.LargeChange = 1;
            this.HScrollLutMin.Location = new System.Drawing.Point(122, 60);
            this.HScrollLutMin.Maximum = 32767;
            this.HScrollLutMin.Name = "HScrollLutMin";
            this.HScrollLutMin.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.HScrollLutMin.Size = new System.Drawing.Size(354, 17);
            this.HScrollLutMin.TabIndex = 6;
            this.HScrollLutMin.TabStop = true;
            this.HScrollLutMin.ValueChanged += new System.EventHandler(this.HScrollLutMin_ValueChanged);
            // 
            // PushAsterisk
            // 
            this.PushAsterisk.BackColor = System.Drawing.SystemColors.Control;
            this.PushAsterisk.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushAsterisk.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushAsterisk.Location = new System.Drawing.Point(484, 40);
            this.PushAsterisk.Name = "PushAsterisk";
            this.PushAsterisk.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushAsterisk.Size = new System.Drawing.Size(34, 34);
            this.PushAsterisk.TabIndex = 7;
            this.PushAsterisk.Text = "*";
            this.PushAsterisk.UseVisualStyleBackColor = false;
            // 
            // PicDisplay
            // 
            this.PicDisplay.BackColor = System.Drawing.SystemColors.Control;
            this.PicDisplay.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.PicDisplay.Cursor = System.Windows.Forms.Cursors.Default;
            this.PicDisplay.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PicDisplay.Location = new System.Drawing.Point(139, 95);
            this.PicDisplay.Name = "PicDisplay";
            this.PicDisplay.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PicDisplay.Size = new System.Drawing.Size(392, 463);
            this.PicDisplay.TabIndex = 8;
            this.PicDisplay.TabStop = false;
            // 
            // PushInit
            // 
            this.PushInit.BackColor = System.Drawing.SystemColors.Control;
            this.PushInit.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushInit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushInit.Location = new System.Drawing.Point(14, 95);
            this.PushInit.Name = "PushInit";
            this.PushInit.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushInit.Size = new System.Drawing.Size(113, 33);
            this.PushInit.TabIndex = 8;
            this.PushInit.Text = "Init";
            this.PushInit.UseVisualStyleBackColor = false;
            this.PushInit.Click += new System.EventHandler(this.PushInit_Click);
            // 
            // PushOpen
            // 
            this.PushOpen.BackColor = System.Drawing.SystemColors.Control;
            this.PushOpen.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushOpen.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushOpen.Location = new System.Drawing.Point(14, 134);
            this.PushOpen.Name = "PushOpen";
            this.PushOpen.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushOpen.Size = new System.Drawing.Size(113, 33);
            this.PushOpen.TabIndex = 9;
            this.PushOpen.Text = "Open";
            this.PushOpen.UseVisualStyleBackColor = false;
            this.PushOpen.Click += new System.EventHandler(this.PushOpen_Click);
            // 
            // PushInfo
            // 
            this.PushInfo.BackColor = System.Drawing.SystemColors.Control;
            this.PushInfo.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushInfo.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushInfo.Location = new System.Drawing.Point(14, 173);
            this.PushInfo.Name = "PushInfo";
            this.PushInfo.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushInfo.Size = new System.Drawing.Size(113, 33);
            this.PushInfo.TabIndex = 10;
            this.PushInfo.Text = "Information";
            this.PushInfo.UseVisualStyleBackColor = false;
            this.PushInfo.Click += new System.EventHandler(this.PushInfo_Click);
            // 
            // PushProperties
            // 
            this.PushProperties.BackColor = System.Drawing.SystemColors.Control;
            this.PushProperties.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushProperties.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushProperties.Location = new System.Drawing.Point(14, 212);
            this.PushProperties.Name = "PushProperties";
            this.PushProperties.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushProperties.Size = new System.Drawing.Size(113, 33);
            this.PushProperties.TabIndex = 11;
            this.PushProperties.Text = "Properties...";
            this.PushProperties.UseVisualStyleBackColor = false;
            this.PushProperties.Click += new System.EventHandler(this.PushProperties_Click);
            // 
            // PushSnap
            // 
            this.PushSnap.BackColor = System.Drawing.SystemColors.Control;
            this.PushSnap.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushSnap.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushSnap.Location = new System.Drawing.Point(14, 272);
            this.PushSnap.Name = "PushSnap";
            this.PushSnap.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushSnap.Size = new System.Drawing.Size(113, 33);
            this.PushSnap.TabIndex = 12;
            this.PushSnap.Text = "Snap";
            this.PushSnap.UseVisualStyleBackColor = false;
            this.PushSnap.Click += new System.EventHandler(this.PushSnap_Click);
            // 
            // PushLive
            // 
            this.PushLive.BackColor = System.Drawing.SystemColors.Control;
            this.PushLive.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushLive.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushLive.Location = new System.Drawing.Point(14, 311);
            this.PushLive.Name = "PushLive";
            this.PushLive.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushLive.Size = new System.Drawing.Size(113, 33);
            this.PushLive.TabIndex = 13;
            this.PushLive.Text = "Live";
            this.PushLive.UseVisualStyleBackColor = false;
            this.PushLive.Click += new System.EventHandler(this.PushLive_Click);
            // 
            // PushFireTrigger
            // 
            this.PushFireTrigger.BackColor = System.Drawing.SystemColors.Control;
            this.PushFireTrigger.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushFireTrigger.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushFireTrigger.Location = new System.Drawing.Point(14, 350);
            this.PushFireTrigger.Name = "PushFireTrigger";
            this.PushFireTrigger.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushFireTrigger.Size = new System.Drawing.Size(113, 33);
            this.PushFireTrigger.TabIndex = 14;
            this.PushFireTrigger.Text = "Fire Trigger";
            this.PushFireTrigger.UseVisualStyleBackColor = false;
            this.PushFireTrigger.Click += new System.EventHandler(this.PushFireTrigger_Click);
            // 
            // PushIdle
            // 
            this.PushIdle.BackColor = System.Drawing.SystemColors.Control;
            this.PushIdle.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushIdle.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushIdle.Location = new System.Drawing.Point(14, 389);
            this.PushIdle.Name = "PushIdle";
            this.PushIdle.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushIdle.Size = new System.Drawing.Size(113, 33);
            this.PushIdle.TabIndex = 15;
            this.PushIdle.Text = "Idle";
            this.PushIdle.UseVisualStyleBackColor = false;
            this.PushIdle.Click += new System.EventHandler(this.PushIdle_Click);
            // 
            // PushBufRelease
            // 
            this.PushBufRelease.BackColor = System.Drawing.SystemColors.Control;
            this.PushBufRelease.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushBufRelease.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushBufRelease.Location = new System.Drawing.Point(14, 428);
            this.PushBufRelease.Name = "PushBufRelease";
            this.PushBufRelease.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushBufRelease.Size = new System.Drawing.Size(113, 33);
            this.PushBufRelease.TabIndex = 16;
            this.PushBufRelease.Text = "Buf Release";
            this.PushBufRelease.UseVisualStyleBackColor = false;
            this.PushBufRelease.Click += new System.EventHandler(this.PushBufRelease_Click);
            // 
            // PushClose
            // 
            this.PushClose.BackColor = System.Drawing.SystemColors.Control;
            this.PushClose.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushClose.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushClose.Location = new System.Drawing.Point(14, 486);
            this.PushClose.Name = "PushClose";
            this.PushClose.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushClose.Size = new System.Drawing.Size(113, 33);
            this.PushClose.TabIndex = 17;
            this.PushClose.Text = "Close";
            this.PushClose.UseVisualStyleBackColor = false;
            this.PushClose.Click += new System.EventHandler(this.PushClose_Click);
            // 
            // PushUninit
            // 
            this.PushUninit.BackColor = System.Drawing.SystemColors.Control;
            this.PushUninit.Cursor = System.Windows.Forms.Cursors.Default;
            this.PushUninit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.PushUninit.Location = new System.Drawing.Point(14, 525);
            this.PushUninit.Name = "PushUninit";
            this.PushUninit.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.PushUninit.Size = new System.Drawing.Size(113, 33);
            this.PushUninit.TabIndex = 18;
            this.PushUninit.Text = "Uninit";
            this.PushUninit.UseVisualStyleBackColor = false;
            this.PushUninit.Click += new System.EventHandler(this.PushUninit_Click);
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 573);
            this.Controls.Add(this.LabelStatus);
            this.Controls.Add(this.LabelLutMin);
            this.Controls.Add(this.LabelLutMax);
            this.Controls.Add(this.EditLutMin);
            this.Controls.Add(this.EditLutMax);
            this.Controls.Add(this.PushAsterisk);
            this.Controls.Add(this.HScrollLutMin);
            this.Controls.Add(this.HScrollLutMax);
            this.Controls.Add(this.PicDisplay);
            this.Controls.Add(this.PushInit);
            this.Controls.Add(this.PushOpen);
            this.Controls.Add(this.PushInfo);
            this.Controls.Add(this.PushProperties);
            this.Controls.Add(this.PushSnap);
            this.Controls.Add(this.PushLive);
            this.Controls.Add(this.PushFireTrigger);
            this.Controls.Add(this.PushIdle);
            this.Controls.Add(this.PushBufRelease);
            this.Controls.Add(this.PushClose);
            this.Controls.Add(this.PushUninit);
            this.Name = "FormMain";
            this.Text = "csAcq4";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.PicDisplay)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label LabelStatus;
        public System.Windows.Forms.Label LabelLutMin;
        public System.Windows.Forms.Label LabelLutMax;
        public System.Windows.Forms.TextBox EditLutMin;
        public System.Windows.Forms.TextBox EditLutMax;
        public System.Windows.Forms.Button PushAsterisk;
        public System.Windows.Forms.HScrollBar HScrollLutMin;
        public System.Windows.Forms.HScrollBar HScrollLutMax;
        public System.Windows.Forms.PictureBox PicDisplay;
        public System.Windows.Forms.Button PushInit;
        public System.Windows.Forms.Button PushOpen;
        public System.Windows.Forms.Button PushInfo;
        public System.Windows.Forms.Button PushProperties;
        public System.Windows.Forms.Button PushSnap;
        public System.Windows.Forms.Button PushLive;
        public System.Windows.Forms.Button PushFireTrigger;
        public System.Windows.Forms.Button PushIdle;
        public System.Windows.Forms.Button PushBufRelease;
        public System.Windows.Forms.Button PushClose;
        public System.Windows.Forms.Button PushUninit;

    }
}