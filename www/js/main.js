const inpFile = document.getElementById('inpFile');
const previewContainer = document.getElementById('imagePreview');
const previewImage = previewContainer.querySelector('.image-preview__image');
const previewDefaultText = previewContainer.querySelector('.image-preview__default-text');

function updateFileName() {
	const fileInput = document.getElementById('inpFile');
	const customFileLabel = document.getElementById('customFileLabel');
  
	if (fileInput.files.length > 0) {
	  customFileLabel.textContent = fileInput.files[0].name;
	} else {
	  customFileLabel.textContent = 'Choose File';
	}
  }

inpFile.addEventListener('change', function () {
    const file = this.files[0];  // usar o files e não o file
	const allowedImageTypes = ["image/jpeg", "image/png", "image/gif"];
    console.log(file);

    if (file) {
		const fileType = file.type;

		if (allowedImageTypes.includes(fileType)) 
		{
        	const reader = new FileReader;
        	console.log(reader); // podemos ver o object file reader que é assincrono
       		previewDefaultText.style.display = 'none';
        	previewImage.style.display = 'block';

        	reader.addEventListener('load', function() {
            	console.log(this); // exactamente o resultado do anterior event.target - o this é o reader
            	previewImage.setAttribute('src', this.result);
		    });

        	reader.readAsDataURL(file);
		} else 
		{
		    // Se o arquivo não for uma imagem, não exiba a prévia
			previewDefaultText.style.display = null;
			previewImage.style.display = null;
			previewImage.setAttribute('src', '');
		}
    } else {
        // colocar o null significa voltar aos valores por defeito do css
        previewDefaultText.style.display = null;
        previewImage.style.display = null;
        previewImage.setAttribute('src', "");
    }
    updateFileName();
});


async function excluirArquivo(event) {
	event.preventDefault();

	const nomeArquivo = document.getElementById('nomeArquivo').value;
	const url = `https://exemplo.com/excluir-arquivo/${encodeURIComponent(nomeArquivo)}`;

	try {
	  const response = await fetch(url, {
		method: 'DELETE',
		headers: {
			'Cache-Control': 'no-cache',
			'Host': 'localhost:8080',
			'Accept-Encoding': 'gzip, deflate, br',
			'Connection': 'keep-alive',
			'Content-Type': 'application/x-www-form-urlencoded',
		  // Adicione outros cabeçalhos, como Authorization, conforme necessário
		},
	  });

	  if (response.ok) {
		console.log(`Arquivo ${nomeArquivo} excluído com sucesso.`);
	  } else {
		console.error(`Erro ao excluir arquivo ${nomeArquivo}. Código de status: ${response.status}`);
	  }
	} catch (error) {
	  console.error(`Erro na solicitação DELETE: ${error}`);
	}
  }