#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace pos {

	/// <summary>
	/// Summary for frmTouchInicial
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmTouchInicial : public System::Windows::Forms::Form
	{
	public:
		frmTouchInicial(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmTouchInicial()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^  pbImg;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmTouchInicial::typeid));
			this->pbImg = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbImg))->BeginInit();
			this->SuspendLayout();
			// 
			// pbImg
			// 
			this->pbImg->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pbImg->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pbImg.Image")));
			this->pbImg->Location = System::Drawing::Point(0, 0);
			this->pbImg->Name = L"pbImg";
			this->pbImg->Size = System::Drawing::Size(792, 566);
			this->pbImg->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pbImg->TabIndex = 0;
			this->pbImg->TabStop = false;
			this->pbImg->Click += gcnew System::EventHandler(this, &frmTouchInicial::pbImg_Click);
			// 
			// frmTouchInicial
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(792, 566);
			this->Controls->Add(this->pbImg);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"frmTouchInicial";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmTouchInicial";
			this->Load += gcnew System::EventHandler(this, &frmTouchInicial::frmTouchInicial_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbImg))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void pbImg_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
	private: System::Void frmTouchInicial_Load(System::Object^  sender, System::EventArgs^  e) {
				 this->WindowState = FormWindowState::Maximized;
			 }
	};
}
