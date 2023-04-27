using System.Diagnostics;
using System.Net;
using System.Net.Sockets;

namespace udp_cam
{
    public partial class Form1 : Form
    {
        private UdpClient udpClient;
        private Thread receiveThread;
        private Bitmap image;
        private object imageLock = new object();
        private bool formClosed = false;

        private UInt32 frame_start_packet = 0xdeadbeef;
        private UInt32 header_pixel_data = 0xbeefbeef;
        private UInt32 frame_end_packet = 0xdeaddead;
        private UInt32 height, width, sz, fcounter = 0; // height, column, data-size
        private uint color, r, g, b;
        private Bitmap bmp;

        public Form1()
        {
            InitializeComponent();
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Debug.WriteLine("Closing Process");

            if (udpClient != null)
            {
                udpClient.Close();
            }

            formClosed = true;
            pictureBox1.Dispose();

            if (receiveThread != null)
            {
                receiveThread.Join();
                Debug.WriteLine("Thread terminated");
            }

        }

        private void ReceiveData()
        {
            bmp = new Bitmap(640, 480); // �o�C�g�z����r�b�g�}�b�v�ɕϊ�
            while (!formClosed)
            {
                try
                {
                    IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, 0);
                    byte[] data = udpClient.Receive(ref endPoint);
                    UInt32 header = BitConverter.ToUInt32(data, 0);


                    if (header == header_pixel_data)
                    {
                        //Debug.WriteLine("data!");
                        height = BitConverter.ToUInt32(data, 4) - 1;    //height number
                        width = BitConverter.ToUInt32(data, 8) - 1;     //columb number
                        sz = BitConverter.ToUInt32(data, 12) * 2;       // data size

                        int index = 16; // �擪��16�o�C�g���̓w�b�_�Ȃ̂ŁAindex��16����n�߂�

                        for (int x = (int)width; x < sz; x++)
                        {
                            // RGB565�̃o�C�i���f�[�^����͂��āA�J���[�����擾
                            color = (uint)(data[index] << 8 | data[index + 1]);
                            r = (color >> 11) & 0x1f;
                            g = (color >> 5) & 0x3f;
                            b = color & 0x1f;
                            r = (r * 255 / 31);
                            g = (g * 255 / 63);
                            b = (b * 255 / 31);

                            // �s�N�Z���ɃJ���[���Z�b�g
                            bmp.SetPixel((int)x, (int)height, Color.FromArgb((int)r, (int)g, (int)b));

                            index += 2;
                        }
                        fcounter++;
                    }
                    //else if (BitConverter.ToUInt32(data, 0) == frame_end_packet)
                    if ((fcounter <= 480) && (header != frame_start_packet))
                    {
                        continue;
                    }
                    fcounter = 0;

                    lock (imageLock)
                    {

                        image = new Bitmap(bmp);
                        pictureBox1.Invoke(new Action(() =>
                        {
                            // 2�{�Ɋg�傷��{�����`���܂��B
                            const int ZoomFactor = 1;

                            // 2�{�̃T�C�Y�ŐV�����r�b�g�}�b�v���쐬���܂��B
                            Bitmap zoomedBitmap = new Bitmap(image.Width * ZoomFactor, image.Height * ZoomFactor);

                            // �V�����r�b�g�}�b�v��Graphics�I�u�W�F�N�g���擾���܂��B
                            Graphics graphics = Graphics.FromImage(zoomedBitmap);

                            // InterpolationMode��HighQualityBicubic�ɐݒ肵�A�摜�̊g��i�������サ�܂��B
                            graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;

                            // �V�����r�b�g�}�b�v�ɃI���W�i���̃r�b�g�}�b�v��2�{�̃T�C�Y�ŕ`�悵�܂��B
                            graphics.DrawImage(image, new Rectangle(0, 0, zoomedBitmap.Width, zoomedBitmap.Height), new Rectangle(0, 0, image.Width, image.Height), GraphicsUnit.Pixel);

                            // PictureBox�ɐV�����r�b�g�}�b�v��\�����܂��B
                            if (pictureBox1.Image != null)
                            {
                                pictureBox1.Image.Dispose();
                                pictureBox1.Image = null;
                            }
                            pictureBox1.Image = zoomedBitmap;
                            pictureBox1.Invalidate();
                        }));
                    }
                }
                catch (Exception ex)
                {
                    Debug.WriteLine(ex);
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (udpClient == null)
            {
                udpClient = new UdpClient(1024);
                receiveThread = new Thread(new ThreadStart(ReceiveData));
                receiveThread.Start();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}

